package com.leapmotion.leap.socket
{
	import com.leapmotion.leap.Finger;
	import com.leapmotion.leap.Frame;
	import com.leapmotion.leap.Hand;
	import com.leapmotion.leap.Pointable;
	import com.leapmotion.leap.Tool;
	import com.leapmotion.leap.Vector3;
	import com.leapmotion.leap.events.LeapMotionEvent;
	import com.leapmotion.leap.events.LeapMotionEventProxy;
	import com.leapmotion.leap.util.Base64Encoder;
	import com.leapmotion.leap.util.SHA1;

	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IOErrorEvent;
	import flash.events.ProgressEvent;
	import flash.events.SecurityErrorEvent;
	import flash.net.Socket;
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	/**
	 * The LeapSocket class handles the communication via WebSockets.
	 *
	 * @author logotype
	 *
	 */
	public class LeapSocket extends EventDispatcher
	{
		static public const STATE_CONNECTING:int = 0;
		static public const STATE_OPEN:int = 1;

		/**
		 * The raw socket connection to Leap 
		 */
		private var socket:Socket;
		
		/**
		 * The current state of the parser 
		 */
		private var currentState:int;
		
		/**
		 * Event Dispatcher singleton 
		 */
		private var eventDispatcher:LeapMotionEventProxy;
		
		/**
		 * Specifies which host to connect to, default localhost 
		 */
		private var host:String = "localhost";

		/**
		 * Number of byts of the handshake response 
		 */
		private var handshakeBytesReceived:int;
		
		/**
		 * The device handshake response from Leap 
		 */
		private var leapMotionDeviceHandshakeResponse:String = "";
		
		/**
		 * Base64 encoded cryptographic nonce value 
		 */
		private var base64nonce:String;
		
		/**
		 * Most recent non-parsed Frame received from Socket 
		 */
		private var leapSocketFrame:LeapSocketFrame = new LeapSocketFrame();
		
		/**
		 * Whether the Leap is currently connected 
		 */
		public var isConnected:Boolean = false;

		public function LeapSocket( host:String = null )
		{
			if( host )
				this.host = host;
			
			eventDispatcher = LeapMotionEventProxy.getInstance();

			// Generate nonce
			var nonce:ByteArray = new ByteArray();
			for ( var i:int = 0; i < 16; i++ )
				nonce.writeByte( Math.round( Math.random() * 0xFF ));

			nonce.position = 0;
			
			var encoder:Base64Encoder = new Base64Encoder();
			encoder.encodeBytes( nonce );
			base64nonce = encoder.flush();

			socket = new Socket( this.host, 6437 );
			socket.addEventListener( Event.CONNECT, onSocketConnectHandler );
			socket.addEventListener( IOErrorEvent.IO_ERROR, onIOErrorHandler );
			socket.addEventListener( SecurityErrorEvent.SECURITY_ERROR, onSecurityErrorHandler );
			socket.addEventListener( ProgressEvent.SOCKET_DATA, onSocketDataHandler );
		}

		/**
		 * Triggered once the Socket-connection is established, send handshake 
		 * @param event
		 * 
		 */
		private function onSocketConnectHandler( event:Event ):void
		{
			isConnected = false;
			eventDispatcher.dispatchEvent( new LeapMotionEvent( LeapMotionEvent.LEAPMOTION_INIT ));
			currentState = LeapSocket.STATE_CONNECTING;
			socket.endian = Endian.BIG_ENDIAN;
			sendHandshake();
		}

		/**
		 * Triggered when network-error occurs 
		 * @param event
		 * 
		 */
		private function onIOErrorHandler( event:IOErrorEvent ):void
		{
			isConnected = false;
			eventDispatcher.dispatchEvent( new LeapMotionEvent( LeapMotionEvent.LEAPMOTION_DISCONNECTED ));
		}

		/**
		 * Triggered if socket policy-file is missing, or other security related error occurs 
		 * @param event
		 * 
		 */
		private function onSecurityErrorHandler( event:SecurityErrorEvent ):void
		{
			isConnected = false;
			eventDispatcher.dispatchEvent( new LeapMotionEvent( LeapMotionEvent.LEAPMOTION_DISCONNECTED ));
		}

		/**
		 * Triggered when the socket-connection is closed 
		 * @param event
		 * 
		 */
		private function onSocketCloseHandler( event:Event ):void
		{
			isConnected = false;
			eventDispatcher.dispatchEvent( new LeapMotionEvent( LeapMotionEvent.LEAPMOTION_DISCONNECTED ));
			eventDispatcher.dispatchEvent( new LeapMotionEvent( LeapMotionEvent.LEAPMOTION_EXIT ));
		}

		/**
		 * Inline method where data is read until a complete LeapSocketFrame is parsed 
		 * @param event
		 * @see LeapSocketFrame
		 * 
		 */
		[Inline]
		final private function onSocketDataHandler( event:ProgressEvent = null ):void
		{
			if ( currentState == LeapSocket.STATE_CONNECTING )
			{
				readLeapMotionHandshake();
				return;
			}

			isConnected = true;

			// Loop until data has been completely added to the frame
			while ( socket.connected && leapSocketFrame.addData( socket ))
			{
				leapSocketFrame.binaryPayload.position = 0;
				var utf8data:String = leapSocketFrame.binaryPayload.readMultiByte( leapSocketFrame.length, "utf-8" );
				var i:uint = 0;
				var j:uint = 0;
				var json:Object = JSON.parse( utf8data );
				var frame:Frame = new Frame();
				var hand:Hand;
				var abstractPointable:Pointable;
				var vector:Vector3;
				var isTool:Boolean;

				// Hands
				if ( json.hands )
				{
					for ( i = 0; i < json.hands.length; ++i )
					{
						hand = new Hand();
						hand.frame = frame;
						hand.direction = new Vector3( json.hands[ i ].direction[ 0 ], json.hands[ i ].direction[ 1 ], json.hands[ i ].direction[ 2 ]);
						hand.id = json.hands[ i ].id;
						hand.palmNormal = new Vector3( json.hands[ i ].palmNormal[ 0 ], json.hands[ i ].palmNormal[ 1 ], json.hands[ i ].palmNormal[ 2 ]);
						hand.palmPosition = new Vector3( json.hands[ i ].palmPosition[ 0 ], json.hands[ i ].palmPosition[ 1 ], json.hands[ i ].palmPosition[ 2 ]);
						hand.palmVelocity = new Vector3( json.hands[ i ].palmPosition[ 0 ], json.hands[ i ].palmPosition[ 1 ], json.hands[ i ].palmPosition[ 2 ]);
						for ( j = 0; j < json.hands[ i ].r.length; ++j )
						{
							vector = new Vector3( json.hands[ i ].r[ j ][ 0 ], json.hands[ i ].r[ j ][ 1 ], json.hands[ i ].r[ j ][ 2 ]);
							hand.r.push( vector );
						}
						hand.s = json.hands[ i ].s;
						hand.sphereCenter = new Vector3( json.hands[ i ].sphereCenter[ 0 ], json.hands[ i ].sphereCenter[ 1 ], json.hands[ i ].sphereCenter[ 2 ]);
						hand.sphereRadius = json.hands[ i ].sphereRadius;
						hand.t = new Vector3( json.hands[ i ].t[ 0 ], json.hands[ i ].t[ 1 ], json.hands[ i ].t[ 2 ]);
						frame.hands.push( hand );

						// Set primary hand
						if ( i == 0 )
							frame.hand = hand;
					}
				}

				// ID
				frame.id = json.id;

				// Pointables
				if ( json.pointables )
				{
					for ( i = 0; i < json.pointables.length; ++i )
					{
						isTool = json.pointables[ i ].tool;
						if ( isTool )
							abstractPointable = new Tool();
						else
							abstractPointable = new Finger();

						abstractPointable.frame = frame;
						abstractPointable.id = json.pointables[ i ].id;
						abstractPointable.hand = getHandByID( frame, int( json.pointables[ i ].handId ));
						abstractPointable.length = json.pointables[ i ].length;
						abstractPointable.direction = new Vector3( json.pointables[ i ].direction[ 0 ], json.pointables[ i ].direction[ 1 ], json.pointables[ i ].direction[ 2 ]);
						abstractPointable.tipPosition = new Vector3( json.pointables[ i ].tipPosition[ 0 ], json.pointables[ i ].tipPosition[ 1 ], json.pointables[ i ].tipPosition[ 2 ]);
						abstractPointable.tipVelocity = new Vector3( json.pointables[ i ].tipVelocity[ 0 ], json.pointables[ i ].tipVelocity[ 1 ], json.pointables[ i ].tipVelocity[ 2 ]);

						if ( isTool )
						{
							abstractPointable.isTool = true;
							abstractPointable.isFinger = false;
							frame.tools.push( abstractPointable );
							if ( frame.hand )
								frame.hand.tools.push( abstractPointable );
						}
						else
						{
							abstractPointable.isTool = false;
							abstractPointable.isFinger = true;
							frame.fingers.push( abstractPointable );
							if ( frame.hand )
								frame.hand.fingers.push( abstractPointable );
						}

						// Set first as primary
						if ( i == 0 )
						{
							if ( isTool )
							{
								frame.tool = Tool( abstractPointable );
								if ( frame.hand )
								{
									frame.hand.tool = Tool( abstractPointable );
									frame.hand.pointable = abstractPointable;
								}
							}
							else
							{
								frame.finger = Finger( abstractPointable );
								if ( frame.hand )
								{
									frame.hand.finger = Finger( abstractPointable );
									frame.hand.pointable = abstractPointable;
								}
							}
						}
					}
				}

				// Rotation (since last frame), interpolate for smoother motion
				if ( json.r )
				{
					for ( i = 0; i < json.r.length; ++i )
					{
						vector = new Vector3( json.r[ i ][ 0 ], json.r[ i ][ 1 ], json.r[ i ][ 2 ]);
						frame.r.push( vector );
					}
				}

				// Scale factor (since last frame), interpolate for smoother motion
				frame.s = json.s;

				// Translation (since last frame), interpolate for smoother motion
				if ( json.t )
					frame.t = new Vector3( json.t[ 0 ], json.t[ 1 ], json.t[ 2 ]);

				// Timestamp
				frame.timestamp = json.timestamp;

				// Dispatches the prepared data
				eventDispatcher.dispatchEvent( new LeapMotionEvent( LeapMotionEvent.LEAPMOTION_FRAME, frame ));

				// Release current frame and create a new one
				leapSocketFrame = new LeapSocketFrame();
			}
		}

		/**
		 * Inline method. Finds a Hand object by ID 
		 * @param frame The Frame object in which the Hand contains
		 * @param id The ID of the Hand object
		 * @return The Hand object if found, otherwise null
		 * 
		 */
		[Inline]
		final private function getHandByID( frame:Frame, id:int ):Hand
		{
			var returnValue:Hand = null;
			for each ( var hand:Hand in frame.hands )
			{
				if ( hand.id == id )
				{
					returnValue = hand;
					break;
				}
			}
			return returnValue;
		}

		/**
		 * Parses the HTTP header received from the Leap 
		 * @param line
		 * @return 
		 * 
		 */
		private function parseHTTPHeader( line:String ):Object
		{
			var header:Array = line.split( /\: +/ );
			return header.length === 2 ? { name:header[ 0 ], value:header[ 1 ]} : null;
		}

		/**
		 * Reads the handshake received from the Leap 
		 * @return 
		 * 
		 */
		private function readHandshakeLine():Boolean
		{
			var char:String;
			while ( socket.bytesAvailable )
			{
				char = socket.readMultiByte( 1, "us-ascii" );
				handshakeBytesReceived++;
				leapMotionDeviceHandshakeResponse += char;
				if ( char == "\n" )
					return true;
			}
			return false;
		}

		/**
		 * Sends the HTTP handshake to the Leap 
		 * 
		 */
		private function sendHandshake():void
		{
			var text:String = "";
			text += "GET / HTTP/1.1\r\n";
			text += "Host: " + this.host + ":6437\r\n";
			text += "Upgrade: websocket\r\n";
			text += "Connection: Upgrade\r\n";
			text += "Sec-WebSocket-Key: " + base64nonce + "\r\n";
			text += "Origin: *\r\n";
			text += "Sec-WebSocket-Version: 13\r\n";
			text += "\r\n";

			socket.writeMultiByte( text, "us-ascii" );
		}

		/**
		 * Reads the handshake response from the Leap 
		 * 
		 */
		private function readLeapMotionHandshake():void
		{
			var upgradeHeader:Boolean = false;
			var connectionHeader:Boolean = false;
			var keyValidated:Boolean = false;
			var headersTerminatorIndex:int = -1;

			// Load in HTTP Header lines until we encounter a double-newline.
			while ( headersTerminatorIndex === -1 && readHandshakeLine())
				headersTerminatorIndex = leapMotionDeviceHandshakeResponse.search( /\r?\n\r?\n/ );

			// Slice off the trailing \r\n\r\n from the handshake data
			leapMotionDeviceHandshakeResponse = leapMotionDeviceHandshakeResponse.slice( 0, headersTerminatorIndex );

			var lines:Array = leapMotionDeviceHandshakeResponse.split( /\r?\n/ );

			// Validate status line
			var responseLine:String = lines.shift();
			var responseLineMatch:Array = responseLine.match( /^(HTTP\/\d\.\d) (\d{3}) ?(.*)$/i );
			var httpVersion:String = responseLineMatch[ 1 ];
			var statusCode:int = parseInt( responseLineMatch[ 2 ], 10 );
			var statusDescription:String = responseLineMatch[ 3 ];

			try
			{
				while ( lines.length > 0 )
				{
					responseLine = lines.shift();
					var header:Object = parseHTTPHeader( responseLine );
					var headerName:String = header.name.toLocaleLowerCase();
					var headerValue:String = header.value.toLocaleLowerCase();
					if ( headerName === "upgrade" && headerValue === "websocket" )
					{
						upgradeHeader = true;
					}
					else if ( headerName === "connection" && headerValue === "upgrade" )
					{
						connectionHeader = true;
					}
					else if ( headerName === "sec-websocket-accept" )
					{
						var expectedKey:String = SHA1.hashToBase64( base64nonce + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" );
						if ( header.value === expectedKey )
							keyValidated = true;
					}
				}
			}
			catch ( error:Error )
			{
				trace( "There was an error while parsing the following HTTP Header line:\n" + responseLine );
				return;
			}

			if ( !upgradeHeader )
			{
				trace( "The Leap Motion response did not include a valid Upgrade: websocket header." );
				return;
			}
			if ( !connectionHeader )
			{
				trace( "The Leap Motion response did not include a valid Connection: upgrade header." );
				return;
			}
			if ( !keyValidated )
			{
				trace( "Unable to validate Leap Motion response for Sec-Websocket-Accept header." );
				return;
			}

			leapMotionDeviceHandshakeResponse = null;
			currentState = LeapSocket.STATE_OPEN;
			eventDispatcher.dispatchEvent( new LeapMotionEvent( LeapMotionEvent.LEAPMOTION_CONNECTED ));
			return;
		}
	}
}
