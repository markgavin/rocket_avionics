#tag Window
Begin Window Window_Main
   BackColor       =   &cFFFFFF00
   Backdrop        =   0
   CloseButton     =   True
   Composite       =   False
   Frame           =   0
   FullScreen      =   False
   HasBackColor    =   False
   Height          =   700
   ImplicitInstance=   True
   LiveResize      =   "True"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   True
   MaxWidth        =   32000
   MenuBar         =   0
   MenuBarVisible  =   True
   MinHeight       =   600
   MinimizeButton  =   True
   MinWidth        =   900
   Placement       =   0
   Resizeable      =   True
   Title           =   "Rocket Avionics Ground Station"
   Visible         =   True
   Width           =   1000
   Begin GroupBox GroupConnection
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Connection"
      Enabled         =   True
      Height          =   100
      Index           =   -2147483648
      Left            =   20
      LockBottom      =   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   2
      TabIndex        =   0
      Top             =   20
      Visible         =   True
      Width           =   280
      Begin PopupMenu PopupSerialPort
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         Left            =   40
         ListIndex       =   0
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   0
         Top             =   50
         Visible         =   True
         Width           =   160
      End
      Begin PushButton ButtonRefreshPorts
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "↻"
         Default         =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         Left            =   205
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   1
         Top             =   50
         Visible         =   True
         Width           =   25
      End
      Begin PushButton ButtonConnect
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Connect"
         Default         =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         Left            =   235
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   2
         Top             =   50
         Visible         =   True
         Width           =   55
      End
      Begin Label LabelConnectionStatus
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         Left            =   40
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   2
         Text            =   "Disconnected"
         TextAlign       =   0
         TextColor       =   &c99999900
         Top             =   80
         Visible         =   True
         Width           =   200
      End
   End
   Begin GroupBox GroupTelemetry
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Live Telemetry"
      Enabled         =   True
      Height          =   180
      Index           =   -2147483648
      Left            =   20
      LockBottom      =   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   2
      TabIndex        =   1
      Top             =   130
      Visible         =   True
      Width           =   280
      Begin Label LabelAltitude
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   30
         Index           =   -2147483648
         InitialParent   =   "GroupTelemetry"
         Left            =   40
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   0
         Text            =   "Altitude: --- m"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   18
         Top             =   160
         Visible         =   True
         Width           =   240
      End
      Begin Label LabelVelocity
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupTelemetry"
         Left            =   40
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   1
         Text            =   "Velocity: --- m/s"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   16
         Top             =   195
         Visible         =   True
         Width           =   240
      End
      Begin Label LabelState
         AutoDeactivate  =   True
         Bold            =   True
         Enabled         =   True
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupTelemetry"
         Left            =   40
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   2
         Text            =   "State: ---"
         TextAlign       =   0
         TextColor       =   &c00660000
         TextSize        =   16
         Top             =   225
         Visible         =   True
         Width           =   240
      End
      Begin Label LabelRssi
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupTelemetry"
         Left            =   40
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   3
         Text            =   "RSSI: --- dBm  SNR: --- dB"
         TextAlign       =   0
         TextColor       =   &c66666600
         TextSize        =   11
         Top             =   260
         Visible         =   True
         Width           =   240
      End
   End
   Begin GroupBox GroupCommands
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Commands"
      Enabled         =   True
      Height          =   120
      Index           =   -2147483648
      Left            =   20
      LockBottom      =   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   2
      TabIndex        =   2
      Top             =   320
      Visible         =   True
      Width           =   280
      Begin PushButton ButtonArm
         AutoDeactivate  =   True
         Bold            =   True
         Cancel          =   False
         Caption         =   "ARM"
         Default         =   False
         Enabled         =   False
         Height          =   40
         Index           =   -2147483648
         InitialParent   =   "GroupCommands"
         Left            =   40
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   0
         Top             =   360
         Visible         =   True
         Width           =   100
      End
      Begin PushButton ButtonDisarm
         AutoDeactivate  =   True
         Bold            =   True
         Cancel          =   False
         Caption         =   "DISARM"
         Default         =   False
         Enabled         =   False
         Height          =   40
         Index           =   -2147483648
         InitialParent   =   "GroupCommands"
         Left            =   160
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   1
         Top             =   360
         Visible         =   True
         Width           =   100
      End
      Begin PushButton ButtonDownload
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Download Flight Data"
         Default         =   False
         Enabled         =   False
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupCommands"
         Left            =   40
         LockBottom      =   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   2
         Top             =   410
         Visible         =   True
         Width           =   220
      End
   End
   Begin AltitudeChartCanvas ChartAltitude
      AllowAutoDeactivate=   True
      AllowFocus      =   False
      AllowFocusRing  =   True
      AllowTabs       =   False
      Backdrop        =   0
      DoubleBuffer    =   False
      Enabled         =   True
      Height          =   420
      Index           =   -2147483648
      Left            =   320
      LockBottom      =   True
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Scope           =   2
      TabIndex        =   3
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   20
      Transparent     =   True
      Visible         =   True
      Width           =   660
   End
   Begin PushButton ButtonConsole
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Debug Console"
      Default         =   False
      Enabled         =   True
      Height          =   25
      Index           =   -2147483648
      Left            =   20
      LockBottom      =   True
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      Scope           =   2
      TabIndex        =   4
      Top             =   455
      Visible         =   True
      Width           =   120
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Open()
		  // Initialize connection
		  pConnection = New FlightConnection
		  AddHandler pConnection.ConnectionChanged, AddressOf HandleConnectionChanged
		  AddHandler pConnection.TelemetryReceived, AddressOf HandleTelemetryReceived
		  AddHandler pConnection.LinkStatusChanged, AddressOf HandleLinkStatusChanged
		  AddHandler pConnection.ErrorReceived, AddressOf HandleErrorReceived

		  // Initialize current flight data
		  pCurrentFlight = New FlightData

		  // Refresh serial ports
		  RefreshSerialPorts()
		End Sub
	#tag EndEvent

	#tag Event
		Sub Close()
		  // Clean up
		  If pConnection <> Nil Then
		    RemoveHandler pConnection.ConnectionChanged, AddressOf HandleConnectionChanged
		    RemoveHandler pConnection.TelemetryReceived, AddressOf HandleTelemetryReceived
		    RemoveHandler pConnection.LinkStatusChanged, AddressOf HandleLinkStatusChanged
		    RemoveHandler pConnection.ErrorReceived, AddressOf HandleErrorReceived
		    pConnection.Disconnect
		  End If
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h21
		Private Sub RefreshSerialPorts()
		  // Refresh the list of available serial ports
		  // Preserve current selection if possible

		  // Remember current selection
		  Var theCurrentSelection As String = ""
		  If PopupSerialPort.SelectedRowIndex >= 0 Then
		    theCurrentSelection = PopupSerialPort.Text
		  End If

		  // Get new port list
		  Var thePorts() As String = FlightConnection.GetSerialPorts()

		  // Rebuild list
		  PopupSerialPort.RemoveAllRows

		  For Each thePort As String In thePorts
		    PopupSerialPort.AddRow(thePort)
		  Next

		  If thePorts.Count = 0 Then
		    PopupSerialPort.AddRow("(No ports found)")
		  End If

		  // Restore selection if port still exists
		  If theCurrentSelection <> "" And theCurrentSelection <> "(No ports found)" Then
		    For i As Integer = 0 To PopupSerialPort.LastRowIndex
		      PopupSerialPort.ListIndex = i
		      If PopupSerialPort.Text = theCurrentSelection Then
		        Exit  // Found it, keep selection here
		      End If
		    Next
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleConnectionChanged(inSender As FlightConnection, inConnected As Boolean)
		  #Pragma Unused inSender

		  If inConnected Then
		    LabelConnectionStatus.Text = "Connected"
		    LabelConnectionStatus.TextColor = &c00AA00
		    ButtonConnect.Caption = "Disconnect"
		    ButtonArm.Enabled = True
		    ButtonDisarm.Enabled = True
		    ButtonDownload.Enabled = True
		  Else
		    LabelConnectionStatus.Text = "Disconnected"
		    LabelConnectionStatus.TextColor = &c999999
		    ButtonConnect.Caption = "Connect"
		    ButtonArm.Enabled = False
		    ButtonDisarm.Enabled = False
		    ButtonDownload.Enabled = False
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleTelemetryReceived(inSender As FlightConnection, inSample As TelemetrySample)
		  #Pragma Unused inSender

		  // Update live telemetry display
		  // Show differential altitude if available (more accurate), otherwise flight-reported altitude
		  Var theAltText As String
		  If inSample.pGroundPressurePa > 0.0 And inSample.pDifferentialAltitudeM <> 0.0 Then
		    // Have ground reference - show differential altitude (more accurate)
		    theAltText = "Alt: " + Format(inSample.pDifferentialAltitudeM, "0.0") + " m"
		  Else
		    theAltText = "Alt: " + Format(inSample.pAltitudeM, "0.0") + " m"
		  End If
		  LabelAltitude.Text = theAltText

		  LabelVelocity.Text = "Velocity: " + Format(inSample.pVelocityMps, "0.0") + " m/s"
		  LabelState.Text = "State: " + inSample.pState.Uppercase

		  // Show RSSI/SNR and ground station altitude (if available)
		  Var theRssiText As String = "RSSI: " + Str(inSample.pRssi) + " dBm  SNR: " + Str(inSample.pSnr) + " dB"
		  If inSample.pGroundAltitudeM <> 0.0 Then
		    theRssiText = theRssiText + "  Gnd: " + Format(inSample.pGroundAltitudeM, "0") + " m"
		  End If
		  LabelRssi.Text = theRssiText

		  // Update state color
		  Select Case inSample.pState
		  Case "armed"
		    LabelState.TextColor = &cFF6600
		  Case "boost", "coast"
		    LabelState.TextColor = &cFF0000
		  Case "apogee", "descent"
		    LabelState.TextColor = &c0066FF
		  Case "landed", "complete"
		    LabelState.TextColor = &c00AA00
		  Else
		    LabelState.TextColor = &c006600
		  End Select

		  // Add sample to current flight
		  pCurrentFlight.AddSample(inSample)

		  // Update chart
		  ChartAltitude.SetFlightData(pCurrentFlight)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleLinkStatusChanged(inSender As FlightConnection, inConnected As Boolean)
		  #Pragma Unused inSender

		  If inConnected Then
		    LabelConnectionStatus.Text = "Link Active"
		    LabelConnectionStatus.TextColor = &c00AA00
		  Else
		    LabelConnectionStatus.Text = "Link Lost"
		    LabelConnectionStatus.TextColor = &cFF0000
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleErrorReceived(inSender As FlightConnection, inCode As String, inMessage As String)
		  #Pragma Unused inSender

		  MessageBox("Error: " + inCode + " - " + inMessage)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub DisplayFlightData(inFlight As FlightData)
		  // Display a loaded flight in the main window
		  If inFlight = Nil Then
		    Return
		  End If

		  // Replace current flight with loaded flight
		  pCurrentFlight = inFlight

		  // Update UI with flight data
		  If inFlight.pSamples.Count > 0 Then
		    Var theLastSample As TelemetrySample = inFlight.pSamples(inFlight.pSamples.LastIndex)
		    LabelAltitude.Text = "Altitude: " + Format(inFlight.pMaxAltitudeM, "0.0") + " m (max)"
		    LabelVelocity.Text = "Velocity: " + Format(inFlight.pMaxVelocityMps, "0.0") + " m/s (max)"
		    LabelState.Text = "State: " + theLastSample.pState.Uppercase
		    LabelState.TextColor = &c00AA00  // Green for loaded flight
		  Else
		    LabelAltitude.Text = "Altitude: --- m"
		    LabelVelocity.Text = "Velocity: --- m/s"
		    LabelState.Text = "State: ---"
		    LabelState.TextColor = &c006600
		  End If

		  // Update chart
		  ChartAltitude.SetFlightData(pCurrentFlight)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SimulateTelemetrySample(inSample As TelemetrySample)
		  // Add simulated telemetry sample and update display
		  // Called from App menu handler for testing

		  // Add to flight data
		  pCurrentFlight.AddSample(inSample)

		  // Update telemetry labels
		  LabelAltitude.Text = "Altitude: " + Format(inSample.pAltitudeM, "0.0") + " m"
		  LabelVelocity.Text = "Velocity: " + Format(inSample.pVelocityMps, "0.0") + " m/s"
		  LabelState.Text = "State: " + inSample.pState.Uppercase
		  LabelRssi.Text = "RSSI: " + Str(inSample.pRssi) + " dBm  SNR: " + Str(inSample.pSnr) + " dB"

		  // Update state color
		  Select Case inSample.pState
		  Case "armed"
		    LabelState.TextColor = &cFF6600
		  Case "boost", "coast"
		    LabelState.TextColor = &cFF0000
		  Case "apogee", "descent"
		    LabelState.TextColor = &c0066FF
		  Case "landed", "complete"
		    LabelState.TextColor = &c00AA00
		  Else
		    LabelState.TextColor = &c006600
		  End Select

		  // Update chart
		  ChartAltitude.SetFlightData(pCurrentFlight)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetSelectedPortName() As String
		  // Return the currently selected port name
		  If PopupSerialPort.SelectedRowIndex >= 0 Then
		    Return PopupSerialPort.Text
		  Else
		    Return ""
		  End If
		End Function
	#tag EndMethod


	#tag Property, Flags = &h0
		pConnection As FlightConnection
	#tag EndProperty

	#tag Property, Flags = &h0
		pCurrentFlight As FlightData
	#tag EndProperty



#tag EndWindowCode

#tag Events ButtonRefreshPorts
	#tag Event
		Sub Action()
		  RefreshSerialPorts()
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonConnect
	#tag Event
		Sub Action()
		  If pConnection.IsConnected Then
		    pConnection.Disconnect
		  Else
		    If PopupSerialPort.SelectedRowIndex >= 0 Then
		      Var thePort As String = PopupSerialPort.Text
		      If thePort <> "(No ports found)" Then
		        Call pConnection.Connect(thePort)
		      End If
		    End If
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonArm
	#tag Event
		Sub Action()
		  If pConnection.IsConnected Then
		    // Confirm arming
		    Var theResult As Integer = MsgBox("ARM the flight computer?" + EndOfLine + EndOfLine + _
		      "This will put the rocket in armed mode, ready for launch detection.", 1, "Confirm Arm")
		    If theResult = 1 Then
		      pConnection.SendArm()
		    End If
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonDisarm
	#tag Event
		Sub Action()
		  If pConnection.IsConnected Then
		    pConnection.SendDisarm()
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonDownload
	#tag Event
		Sub Action()
		  If pConnection.IsConnected Then
		    pConnection.SendDownload()
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonConsole
	#tag Event
		Sub Action()
		  // Show debug console window
		  Window_Console.Show
		End Sub
	#tag EndEvent
#tag EndEvents
