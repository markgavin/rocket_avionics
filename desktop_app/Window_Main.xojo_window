#tag Window
Begin Window Window_Main
   BackColor       =   &cFFFFFF00
   Backdrop        =   0
   CloseButton     =   True
   Composite       =   False
   Frame           =   0
   FullScreen      =   False
   HasBackColor    =   False
   HasFullScreenButton=   False
   Height          =   470
   ImplicitInstance=   True
   LiveResize      =   "True"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   True
   MaxWidth        =   32000
   MenuBar         =   0
   MenuBarVisible  =   True
   MinHeight       =   470
   MinimizeButton  =   True
   MinWidth        =   1000
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
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   100
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   2
      TabIndex        =   0
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   20
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   280
      Begin PopupMenu PopupSerialPort
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         InitialValue    =   ""
         Italic          =   False
         Left            =   40
         ListIndex       =   0
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   2
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   50
         Transparent     =   False
         Underline       =   False
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
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         Italic          =   False
         Left            =   205
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   50
         Transparent     =   False
         Underline       =   False
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
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         Italic          =   False
         Left            =   235
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   50
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   55
      End
      Begin Label LabelConnectionStatus
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Disconnected"
         TextAlign       =   0
         TextColor       =   &c99999900
         Tooltip         =   ""
         Top             =   80
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   200
      End
   End
   Begin GroupBox GroupTelemetry
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Live Telemetry"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   180
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   2
      TabIndex        =   1
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   130
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   280
      Begin Label LabelAltitude
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontUnit        =   0
         Height          =   30
         Index           =   -2147483648
         InitialParent   =   "GroupTelemetry"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Altitude: --- m"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   18.0
         Tooltip         =   ""
         Top             =   160
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   240
      End
      Begin Label LabelVelocity
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontUnit        =   0
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupTelemetry"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Velocity: --- m/s"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   16.0
         Tooltip         =   ""
         Top             =   195
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   240
      End
      Begin Label LabelState
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontUnit        =   0
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupTelemetry"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "State: ---"
         TextAlign       =   0
         TextColor       =   &c00660000
         TextSize        =   16.0
         Tooltip         =   ""
         Top             =   225
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   240
      End
      Begin Label LabelRssi
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupTelemetry"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "RSSI: --- dBm  SNR: --- dB"
         TextAlign       =   0
         TextColor       =   &c66666600
         TextSize        =   11.0
         Tooltip         =   ""
         Top             =   260
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   240
      End
   End
   Begin GroupBox GroupCommands
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Commands"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   120
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   2
      TabIndex        =   2
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   320
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   280
      Begin PushButton ButtonArm
         AutoDeactivate  =   True
         Bold            =   True
         Cancel          =   False
         Caption         =   "ARM"
         Default         =   False
         Enabled         =   False
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   40
         Index           =   -2147483648
         InitialParent   =   "GroupCommands"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   360
         Transparent     =   False
         Underline       =   False
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
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   40
         Index           =   -2147483648
         InitialParent   =   "GroupCommands"
         Italic          =   False
         Left            =   160
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   360
         Transparent     =   False
         Underline       =   False
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
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupCommands"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   410
         Transparent     =   False
         Underline       =   False
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
      InitialParent   =   ""
      Left            =   320
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      pChartMargin    =   50
      pDarkMode       =   False
      pShowApogee     =   True
      pShowGrid       =   True
      pShowVelocity   =   False
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
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Close()
		  // Save window position
		  Module_WindowSettings.SaveWindowPosition(Self, "Window_Main")
		  
		  // Clean up
		  If pConnection <> Nil Then
		    RemoveHandler pConnection.ConnectionChanged, AddressOf HandleConnectionChanged
		    RemoveHandler pConnection.TelemetryReceived, AddressOf HandleTelemetryReceived
		    RemoveHandler pConnection.LinkStatusChanged, AddressOf HandleLinkStatusChanged
		    RemoveHandler pConnection.ErrorReceived, AddressOf HandleErrorReceived
		    RemoveHandler pConnection.StorageListReceived, AddressOf HandleStorageListReceived
		    RemoveHandler pConnection.StorageDataReceived, AddressOf HandleStorageDataReceived
		    RemoveHandler pConnection.DeviceInfoReceived, AddressOf HandleDeviceInfoReceived
		    pConnection.Disconnect
		  End If
		End Sub
	#tag EndEvent

	#tag Event
		Sub Open()
		  // Restore window position
		  Module_WindowSettings.LoadWindowPosition(Self, "Window_Main")

		  // Center splash screen over main window and show it
		  Window_Splash.Left = Self.Left + (Self.Width - Window_Splash.Width) / 2
		  Window_Splash.Top = Self.Top + (Self.Height - Window_Splash.Height) / 2
		  Window_Splash.Show

		  // Initialize connection
		  pConnection = New FlightConnection
		  AddHandler pConnection.ConnectionChanged, AddressOf HandleConnectionChanged
		  AddHandler pConnection.TelemetryReceived, AddressOf HandleTelemetryReceived
		  AddHandler pConnection.LinkStatusChanged, AddressOf HandleLinkStatusChanged
		  AddHandler pConnection.ErrorReceived, AddressOf HandleErrorReceived
		  AddHandler pConnection.StorageListReceived, AddressOf HandleStorageListReceived
		  AddHandler pConnection.StorageDataReceived, AddressOf HandleStorageDataReceived
		  AddHandler pConnection.DeviceInfoReceived, AddressOf HandleDeviceInfoReceived

		  // Initialize current flight data
		  pCurrentFlight = New FlightData

		  // Refresh serial ports
		  RefreshSerialPorts()
		End Sub
	#tag EndEvent


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
		Function GetSelectedPortName() As String
		  // Return the currently selected port name
		  If PopupSerialPort.SelectedRowIndex >= 0 Then
		    Return PopupSerialPort.Text
		  Else
		    Return ""
		  End If
		End Function
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
		Private Sub HandleDeviceInfoReceived(inSender As FlightConnection, inIsGateway As Boolean, inInfo As Dictionary)
		  #Pragma Unused inSender
		  
		  // Log that we received device info
		  Window_Console.LogMessage("** Device info received: " + If(inIsGateway, "Gateway", "Flight Computer"))
		  
		  // Display device info in dialog window
		  Try
		    If inIsGateway Then
		      Window_Console.LogMessage("** Calling ShowGatewayInfo...")
		      Window_DeviceInfo.ShowGatewayInfo(inInfo)
		      Window_Console.LogMessage("** ShowGatewayInfo completed")
		    Else
		      Window_Console.LogMessage("** Calling ShowFlightComputerInfo...")
		      Window_DeviceInfo.ShowFlightComputerInfo(inInfo)
		      Window_Console.LogMessage("** ShowFlightComputerInfo completed")
		    End If
		  Catch theError As RuntimeException
		    Window_Console.LogMessage("** ERROR in ShowDeviceInfo: " + theError.Message)
		  End Try
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleErrorReceived(inSender As FlightConnection, inCode As String, inMessage As String)
		  #Pragma Unused inSender
		  
		  MessageBox("Error: " + inCode + " - " + inMessage)
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
		Private Sub HandleStorageDataReceived(inSender As FlightConnection, inIsSd As Boolean, inOffset As Integer, inTotal As Integer, inData As String)
		  #Pragma Unused inSender
		  
		  // Forward to device storage window if visible
		  If Window_DeviceStorage.Visible Then
		    Window_DeviceStorage.HandleStorageData(inIsSd, inOffset, inTotal, inData)
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleStorageListReceived(inSender As FlightConnection, inIsSd As Boolean, inFiles() As Dictionary)
		  #Pragma Unused inSender
		  
		  // Forward to device storage window if visible
		  If Window_DeviceStorage.Visible Then
		    Window_DeviceStorage.HandleStorageList(inIsSd, inFiles)
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
		  
		  // Detect new flight starting (transition to armed or boost)
		  If (inSample.pState = "armed" Or inSample.pState = "boost") And _
		    (pPreviousState = "idle" Or pPreviousState = "complete" Or pPreviousState = "") Then
		    // New flight starting - reset flight data and saved flag
		    pCurrentFlight = New FlightData
		    pFlightSaved = False
		    Window_Console.LogMessage("** New flight started")
		  End If
		  
		  // Add sample to current flight
		  pCurrentFlight.AddSample(inSample)
		  
		  // Auto-save when landing detected
		  If (inSample.pState = "landed" Or inSample.pState = "complete") And Not pFlightSaved Then
		    If pCurrentFlight.GetSampleCount > 10 Then  // Only save if we have meaningful data
		      // Calculate flight time from samples
		      If pCurrentFlight.pSamples.Count > 0 Then
		        pCurrentFlight.pFlightTimeMs = pCurrentFlight.pSamples(pCurrentFlight.pSamples.LastIndex).pTimeMs
		      End If
		      
		      // Save to database
		      If App.pDatabase.SaveFlight(pCurrentFlight) Then
		        pFlightSaved = True
		        Window_Console.LogMessage("** Flight auto-saved: " + pCurrentFlight.pFlightId)
		        Window_Console.LogMessage("   Max altitude: " + Format(pCurrentFlight.pMaxAltitudeM, "0.0") + " m")
		        Window_Console.LogMessage("   Max velocity: " + Format(pCurrentFlight.pMaxVelocityMps, "0.0") + " m/s")
		        Window_Console.LogMessage("   Samples: " + Str(pCurrentFlight.GetSampleCount))
		        // Refresh history window if visible
		        If Window_History.Visible Then
		          Window_History.RefreshFlightList
		        End If
		      Else
		        Window_Console.LogMessage("** ERROR: Failed to auto-save flight")
		      End If
		    End If
		  End If
		  
		  // Track state for transition detection
		  pPreviousState = inSample.pState
		  
		  // Update chart
		  ChartAltitude.SetFlightData(pCurrentFlight)
		End Sub
	#tag EndMethod

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


	#tag Property, Flags = &h0
		pConnection As FlightConnection
	#tag EndProperty

	#tag Property, Flags = &h0
		pCurrentFlight As FlightData
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pFlightSaved As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pPreviousState As String = ""
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
