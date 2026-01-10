#tag Class
Protected Class FlightConnection
	#tag Method, Flags = &h0
		Sub Constructor()
		  // Initialize TCP socket
		  pSocket = New TCPSocket
		  AddHandler pSocket.Connected, AddressOf HandleConnected
		  AddHandler pSocket.DataAvailable, AddressOf HandleDataAvailable
		  AddHandler pSocket.Error, AddressOf HandleError

		  pIsConnected = False
		  pReceiveBuffer = ""
		  pCommandId = 0
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Destructor()
		  // Clean up
		  If pSocket <> Nil Then
		    RemoveHandler pSocket.Connected, AddressOf HandleConnected
		    RemoveHandler pSocket.DataAvailable, AddressOf HandleDataAvailable
		    RemoveHandler pSocket.Error, AddressOf HandleError
		    If pSocket.IsConnected Then
		      pSocket.Close
		    End If
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Connect(inHost As String, inPort As Integer)
		  // Connect to gateway server via TCP
		  If pSocket.IsConnected Then
		    pSocket.Close
		  End If

		  pSocket.Address = inHost
		  pSocket.Port = inPort
		  pSocket.Connect
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Disconnect()
		  // Disconnect from server
		  If pSocket <> Nil And pSocket.IsConnected Then
		    pSocket.Close
		  End If
		  pIsConnected = False
		  RaiseEvent ConnectionChanged(False)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function IsConnected() As Boolean
		  Return pIsConnected
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendArm()
		  // Send arm command
		  SendCommand("arm")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendDisarm()
		  // Send disarm command
		  SendCommand("disarm")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendStatus()
		  // Request status
		  SendCommand("status")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendDownload()
		  // Request flight data download
		  SendCommand("download")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub SendCommand(inCommand As String)
		  // Send a JSON command to the gateway
		  If Not pIsConnected Then Return

		  pCommandId = pCommandId + 1
		  Var theJson As New JSONItem
		  theJson.Value("cmd") = inCommand
		  theJson.Value("id") = pCommandId

		  pSocket.Write(theJson.ToString + EndOfLine)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleConnected(inSender As TCPSocket)
		  #Pragma Unused inSender
		  pIsConnected = True
		  RaiseEvent ConnectionChanged(True)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleDataAvailable(inSender As TCPSocket)
		  // Read available data
		  pReceiveBuffer = pReceiveBuffer + inSender.ReadAll

		  // Process complete lines
		  Var theLineEnd As Integer = pReceiveBuffer.IndexOf(EndOfLine)
		  While theLineEnd >= 0
		    Var theLine As String = pReceiveBuffer.Left(theLineEnd)
		    pReceiveBuffer = pReceiveBuffer.Middle(theLineEnd + EndOfLine.Length)

		    If theLine.Trim <> "" Then
		      ProcessMessage(theLine)
		    End If

		    theLineEnd = pReceiveBuffer.IndexOf(EndOfLine)
		  Wend
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleError(inSender As TCPSocket, theError As RuntimeException)
		  #Pragma Unused inSender
		  pIsConnected = False
		  RaiseEvent ConnectionChanged(False)
		  RaiseEvent ErrorReceived("NET", theError.Message)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub ProcessMessage(inMessage As String)
		  // Parse and process JSON message from gateway
		  Try
		    Var theJson As New JSONItem(inMessage)

		    If theJson.HasKey("type") Then
		      Var theType As String = theJson.Value("type")

		      Select Case theType
		      Case "tel"
		        // Telemetry packet
		        Var theSample As New TelemetrySample
		        theSample.pTimeMs = theJson.Lookup("t", 0)
		        theSample.pAltitudeM = theJson.Lookup("alt", 0.0)
		        theSample.pVelocityMps = theJson.Lookup("vel", 0.0)
		        theSample.pPressurePa = theJson.Lookup("pres", 0.0)
		        theSample.pTemperatureC = theJson.Lookup("temp", 0.0)
		        theSample.pState = theJson.Lookup("state", "")
		        theSample.pRssi = theJson.Lookup("rssi", 0)
		        theSample.pSnr = theJson.Lookup("snr", 0)
		        RaiseEvent TelemetryReceived(theSample)

		      Case "link"
		        // Link status change
		        Var theConnected As Boolean = theJson.Lookup("connected", False)
		        RaiseEvent LinkStatusChanged(theConnected)

		      Case "ack"
		        // Command acknowledgment
		        Var theCmd As String = theJson.Lookup("cmd", "")
		        Var theSuccess As Boolean = theJson.Lookup("ok", False)
		        RaiseEvent CommandAcknowledged(theCmd, theSuccess)

		      Case "error"
		        // Error message
		        Var theCode As String = theJson.Lookup("code", "")
		        Var theMessage As String = theJson.Lookup("msg", "")
		        RaiseEvent ErrorReceived(theCode, theMessage)

		      End Select
		    End If

		  Catch theError As JSONException
		    // Invalid JSON, ignore
		  End Try
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pSocket As TCPSocket
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pIsConnected As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pReceiveBuffer As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pCommandId As Integer = 0
	#tag EndProperty


	#tag Hook, Flags = &h0
		Event ConnectionChanged(inConnected As Boolean)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event TelemetryReceived(inSample As TelemetrySample)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event LinkStatusChanged(inConnected As Boolean)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event CommandAcknowledged(inCommand As String, inSuccess As Boolean)
	#tag EndHook

	#tag Hook, Flags = &h0
		Event ErrorReceived(inCode As String, inMessage As String)
	#tag EndHook


End Class
#tag EndClass
