#tag Window
Begin Window Window_Console
   BackColor       =   &cFFFFFF00
   Backdrop        =   0
   CloseButton     =   True
   Composite       =   False
   Frame           =   0
   FullScreen      =   False
   HasBackColor    =   False
   HasFullScreenButton=   False
   Height          =   570
   ImplicitInstance=   True
   LiveResize      =   "True"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   True
   MaxWidth        =   32000
   MenuBar         =   4294967314
   MenuBarVisible  =   True
   MinHeight       =   400
   MinimizeButton  =   True
   MinWidth        =   500
   Placement       =   0
   Resizeable      =   True
   Title           =   "Debug Console"
   Visible         =   False
   Width           =   750
   Begin GroupBox GroupConnectionInfo
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Connection"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   70
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   10
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Scope           =   2
      TabIndex        =   0
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   10
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   730
      Begin Label LabelPortName
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnectionInfo"
         Italic          =   False
         Left            =   25
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
         Text            =   "Port: Not connected"
         TextAlign       =   0
         TextColor       =   &c66666600
         TextSize        =   12.0
         Tooltip         =   ""
         Top             =   35
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   250
      End
      Begin Label LabelConnectionState
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnectionInfo"
         Italic          =   False
         Left            =   280
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
         Text            =   "Status: Disconnected"
         TextAlign       =   0
         TextColor       =   &cCC000000
         TextSize        =   12.0
         Tooltip         =   ""
         Top             =   35
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   180
      End
      Begin Label LabelLinkState
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnectionInfo"
         Italic          =   False
         Left            =   465
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
         Text            =   "Link: ---"
         TextAlign       =   0
         TextColor       =   &c66666600
         TextSize        =   12.0
         Tooltip         =   ""
         Top             =   35
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   120
      End
      Begin Label LabelSignal
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnectionInfo"
         Italic          =   False
         Left            =   590
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Signal: ---"
         TextAlign       =   0
         TextColor       =   &c66666600
         TextSize        =   11.0
         Tooltip         =   ""
         Top             =   35
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   140
      End
   End
   Begin GroupBox GroupQuickCommands
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Quick Commands"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   55
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   10
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Scope           =   2
      TabIndex        =   1
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   85
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   730
      Begin PushButton ButtonPing
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Ping"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupQuickCommands"
         Italic          =   False
         Left            =   25
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
         Top             =   107
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   70
      End
      Begin PushButton ButtonStatus
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Status"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupQuickCommands"
         Italic          =   False
         Left            =   105
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
         Top             =   107
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   70
      End
      Begin PushButton ButtonArm
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Arm"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupQuickCommands"
         Italic          =   False
         Left            =   195
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
         Top             =   107
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   70
      End
      Begin PushButton ButtonDisarm
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Disarm"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupQuickCommands"
         Italic          =   False
         Left            =   275
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   107
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   70
      End
      Begin PushButton ButtonDownload
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Download"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupQuickCommands"
         Italic          =   False
         Left            =   355
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   4
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   107
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   80
      End
      Begin PushButton ButtonReset
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Reset"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   25
         Index           =   -2147483648
         InitialParent   =   "GroupQuickCommands"
         Italic          =   False
         Left            =   445
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   5
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   107
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   70
      End
   End
   Begin TextArea TextAreaLog
      AllowFocusRing  =   True
      AllowSpellChecking=   True
      AllowStyledText =   True
      AllowTabs       =   False
      AutoDeactivate  =   True
      BackgroundColor =   &cFFFFFF00
      Bold            =   False
      Border          =   True
      Enabled         =   True
      FontName        =   "Menlo"
      FontSize        =   11.0
      FontUnit        =   0
      Format          =   ""
      HasHorizontalScrollbar=   True
      HasVerticalScrollbar=   True
      Height          =   338
      HideSelection   =   True
      Index           =   -2147483648
      Italic          =   False
      Left            =   10
      LimitText       =   0
      LineHeight      =   0.0
      LineSpacing     =   1.0
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Multiline       =   True
      ReadOnly        =   True
      Scope           =   2
      TabIndex        =   2
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   ""
      TextAlignment   =   0
      TextColor       =   &c00000000
      Tooltip         =   ""
      Top             =   150
      Transparent     =   False
      Underline       =   False
      UnicodeMode     =   0
      ValidationMask  =   ""
      Visible         =   True
      Width           =   730
   End
   Begin TextField TextFieldCommand
      AllowFocusRing  =   True
      AllowSpellChecking=   False
      AllowTabs       =   False
      AutoDeactivate  =   True
      BackgroundColor =   &cFFFFFF00
      Bold            =   False
      Border          =   True
      Enabled         =   True
      FontName        =   "Menlo"
      FontSize        =   12.0
      FontUnit        =   0
      Format          =   ""
      Height          =   25
      Hint            =   "Enter JSON command (e.g. {""cmd"":""ping"",""id"":1})"
      Index           =   -2147483648
      Italic          =   False
      Left            =   10
      LimitText       =   0
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      Password        =   False
      ReadOnly        =   False
      Scope           =   2
      TabIndex        =   3
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   ""
      TextAlignment   =   0
      TextColor       =   &c00000000
      Tooltip         =   ""
      Top             =   500
      Transparent     =   False
      Underline       =   False
      ValidationMask  =   ""
      Visible         =   True
      Width           =   640
   End
   Begin PushButton ButtonSend
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Send"
      Default         =   True
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   25
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   660
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   2
      TabIndex        =   4
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   500
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   80
   End
   Begin PushButton ButtonClear
      AutoDeactivate  =   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Clear Log"
      Default         =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   10
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   2
      TabIndex        =   5
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   530
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   80
   End
   Begin Label LabelStats
      AutoDeactivate  =   True
      Bold            =   False
      DataField       =   ""
      DataSource      =   ""
      Enabled         =   True
      FontName        =   "System"
      FontUnit        =   0
      Height          =   16
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   100
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      Multiline       =   False
      Scope           =   2
      Selectable      =   False
      TabIndex        =   6
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "RX: 0  |  TX: 0  |  Errors: 0"
      TextAlign       =   0
      TextColor       =   &c66666600
      TextSize        =   11.0
      Tooltip         =   ""
      Top             =   532
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   400
   End
   Begin CheckBox CheckAutoScroll
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Auto-scroll"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   650
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   False
      Scope           =   2
      State           =   1
      TabIndex        =   7
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   530
      Transparent     =   False
      Underline       =   False
      Value           =   True
      Visible         =   True
      Width           =   90
   End
   Begin CheckBox CheckHideTelemetry
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Hide unchanged telemetry"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      HelpTag         =   "Only show telemetry when state, altitude, or velocity changes significantly"
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   510
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   False
      Scope           =   2
      State           =   0
      TabIndex        =   8
      TabPanelIndex   =   0
      TabStop         =   True
      Top             =   530
      Transparent     =   False
      Underline       =   False
      Value           =   False
      Visible         =   True
      Width           =   135
   End
   Begin Timer TimerRefresh
      Enabled         =   True
      Index           =   -2147483648
      LockedInPosition=   False
      Period          =   500
      RunMode         =   2
      Scope           =   2
      TabPanelIndex   =   0
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Close()
		  // Save window position
		  Module_WindowSettings.SaveWindowPosition(Self, "Window_Console")
		  
		  // Remove handlers from the same connection object they were added to
		  If pHandlerConnection <> Nil And pHandlersAdded Then
		    RemoveHandler pHandlerConnection.RawMessageReceived, AddressOf HandleRawMessage
		    RemoveHandler pHandlerConnection.DebugLog, AddressOf HandleDebugLog
		    RemoveHandler pHandlerConnection.GatewayStatusReceived, AddressOf HandleGatewayStatus
		    pHandlersAdded = False
		    pHandlerConnection = Nil
		  End If
		End Sub
	#tag EndEvent

	#tag Event
		Sub Open()
		  // Restore window position
		  Module_WindowSettings.LoadWindowPosition(Self, "Window_Console")
		  
		  // Set up handlers for raw message logging and debug output
		  // Only add handlers if not already added
		  If Window_Main.pConnection <> Nil And Not pHandlersAdded Then
		    pHandlerConnection = Window_Main.pConnection
		    AddHandler pHandlerConnection.RawMessageReceived, AddressOf HandleRawMessage
		    AddHandler pHandlerConnection.DebugLog, AddressOf HandleDebugLog
		    AddHandler pHandlerConnection.GatewayStatusReceived, AddressOf HandleGatewayStatus
		    pHandlersAdded = True
		  End If
		  
		  // Initial connection state update
		  UpdateConnectionInfo()
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h21
		Private Sub HandleDebugLog(inSender As FlightConnection, inMessage As String)
		  #Pragma Unused inSender
		  
		  // Log debug message
		  LogMessage(inMessage)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleGatewayStatus(inSender As FlightConnection, inConnected As Boolean, inRxCount As Integer, inTxCount As Integer, inRssi As Integer, inSnr As Integer)
		  #Pragma Unused inSender
		  
		  // Update from gateway status response
		  pLastRssi = inRssi
		  pLastSnr = inSnr
		  pLinkActive = inConnected
		  LabelSignal.Text = "RSSI: " + Str(inRssi) + " SNR: " + Str(inSnr)
		  
		  LogMessage("** Gateway status: link=" + If(inConnected, "active", "inactive") + " rx=" + Str(inRxCount) + " tx=" + Str(inTxCount))
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub HandleRawMessage(inSender As FlightConnection, inMessage As String)
		  #Pragma Unused inSender
		  
		  // Update stats and check filtering
		  Try
		    Var theJson As New JSONItem(inMessage)
		    If theJson.HasKey("type") And theJson.Value("type") = "tel" Then
		      pRxCount = pRxCount + 1
		      pLastRssi = theJson.Lookup("rssi", 0)
		      pLastSnr = theJson.Lookup("snr", 0)
		      UpdateStats()
		      
		      // Filter unchanged telemetry if checkbox is checked
		      If CheckHideTelemetry.Value And Not ShouldShowTelemetry(theJson) Then
		        Return  // Skip logging this message
		      End If
		      
		      // Log the telemetry message
		      LogMessage("<< " + inMessage)
		      
		      // Show BMP390 details if enabled
		      If pShowBMP390Details Then
		        Var theFlightPress As Double = theJson.Lookup("pres", 0.0)
		        Var theFlightAlt As Double = theJson.Lookup("alt", 0.0)
		        Var theFlightTemp As Double = theJson.Lookup("temp", 0.0)
		        Var theGatewayPress As Double = theJson.Lookup("gpres", 0.0)
		        Var theGatewayAlt As Double = theJson.Lookup("galt", 0.0)
		        Var theDiffAlt As Double = theJson.Lookup("dalt", 0.0)
		        
		        Var theBmpInfo As String = "   BMP390 Details:" + EndOfLine
		        theBmpInfo = theBmpInfo + "     Flight:  Pressure=" + Format(theFlightPress, "#,###.0") + " Pa, "
		        theBmpInfo = theBmpInfo + "Alt=" + Format(theFlightAlt, "#,##0.00") + " m, "
		        theBmpInfo = theBmpInfo + "Temp=" + Format(theFlightTemp, "#0.0") + " C" + EndOfLine
		        theBmpInfo = theBmpInfo + "     Gateway: Pressure=" + Format(theGatewayPress, "#,###.0") + " Pa, "
		        theBmpInfo = theBmpInfo + "Alt=" + Format(theGatewayAlt, "#,##0.00") + " m" + EndOfLine
		        theBmpInfo = theBmpInfo + "     Differential Alt=" + Format(theDiffAlt, "#,##0.00") + " m (flight above gateway)"
		        
		        LogMessage(theBmpInfo)
		      End If
		    ElseIf theJson.HasKey("type") And theJson.Value("type") = "ack" Then
		      pRxCount = pRxCount + 1
		      UpdateStats()
		      LogMessage("<< " + inMessage)
		    ElseIf theJson.HasKey("type") And theJson.Value("type") = "status" Then
		      pRxCount = pRxCount + 1
		      UpdateStats()
		      LogMessage("<< " + inMessage)
		    Else
		      // Other message types - always log
		      LogMessage("<< " + inMessage)
		    End If
		  Catch
		    // Non-JSON message - always log
		    LogMessage("<< " + inMessage)
		  End Try
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub LogMessage(inMessage As String)
		  // Add timestamped message to log
		  Var theTimestamp As String = DateTime.Now.ToString("HH:mm:ss.fff")
		  Var theLine As String = "[" + theTimestamp + "] " + inMessage
		  
		  If TextAreaLog.Text <> "" Then
		    TextAreaLog.Text = TextAreaLog.Text + EndOfLine + theLine
		  Else
		    TextAreaLog.Text = theLine
		  End If
		  
		  // Auto-scroll to bottom
		  If CheckAutoScroll.Value Then
		    TextAreaLog.SelectionStart = TextAreaLog.Text.Length
		  End If
		  
		  // Limit log size (keep last 1000 lines)
		  Var theLines() As String = TextAreaLog.Text.Split(EndOfLine)
		  If theLines.Count > 1000 Then
		    Var theRemoveCount As Integer = theLines.Count - 1000
		    For i As Integer = 1 To theRemoveCount
		      theLines.RemoveAt(0)
		    Next
		    TextAreaLog.Text = String.FromArray(theLines, EndOfLine)
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SendRawCommand(inCommand As String)
		  // Send raw command and log it
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendRaw(inCommand)
		    LogMessage(">> " + inCommand)
		    pTxCount = pTxCount + 1
		    UpdateStats()
		  Else
		    LogMessage("!! Not connected - cannot send")
		    pErrorCount = pErrorCount + 1
		    UpdateStats()
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function ShouldShowTelemetry(inJson As JSONItem) As Boolean
		  // Check if telemetry has changed significantly
		  // Returns True if message should be shown, False to filter it out
		  
		  Var theState As String = inJson.Lookup("state", "")
		  Var theAlt As Double = inJson.Lookup("alt", 0.0)
		  Var theVel As Double = inJson.Lookup("vel", 0.0)
		  
		  Var theChanged As Boolean = False
		  
		  // State change is always significant
		  If theState <> pLastTelState Then
		    theChanged = True
		    pLastTelState = theState
		  End If
		  
		  // Altitude change > 1m is significant
		  If Abs(theAlt - pLastTelAlt) > 1.0 Then
		    theChanged = True
		    pLastTelAlt = theAlt
		  End If
		  
		  // Velocity change > 1 m/s is significant
		  If Abs(theVel - pLastTelVel) > 1.0 Then
		    theChanged = True
		    pLastTelVel = theVel
		  End If
		  
		  Return theChanged
		End Function
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub UpdateConnectionInfo()
		  // Update connection info labels
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    // Get port name using public method
		    Var thePortName As String = Window_Main.GetSelectedPortName()
		    If thePortName <> "" Then
		      LabelPortName.Text = "Port: " + thePortName
		      LabelPortName.TextColor = &c000000
		    Else
		      LabelPortName.Text = "Port: Connected"
		      LabelPortName.TextColor = &c000000
		    End If
		    LabelConnectionState.Text = "Status: Connected"
		    LabelConnectionState.TextColor = &c00AA00
		  Else
		    LabelPortName.Text = "Port: Not connected"
		    LabelPortName.TextColor = &c666666
		    LabelConnectionState.Text = "Status: Disconnected"
		    LabelConnectionState.TextColor = &cCC0000
		    LabelLinkState.Text = "Link: ---"
		    LabelLinkState.TextColor = &c666666
		    LabelSignal.Text = "Signal: ---"
		    LabelSignal.TextColor = &c666666
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub UpdateStats()
		  LabelStats.Text = "RX: " + Str(pRxCount) + "  |  TX: " + Str(pTxCount) + "  |  Errors: " + Str(pErrorCount) + "  |  RSSI: " + Str(pLastRssi) + " dBm  SNR: " + Str(pLastSnr) + " dB"
		  LabelSignal.Text = "RSSI: " + Str(pLastRssi) + " SNR: " + Str(pLastSnr)
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pErrorCount As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pHandlerConnection As FlightConnection
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pHandlersAdded As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pLastRssi As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pLastSnr As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pLastTelAlt As Double = -9999
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pLastTelState As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pLastTelVel As Double = -9999
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pLinkActive As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pRxCount As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pShowBMP390Details As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pTxCount As Integer = 0
	#tag EndProperty


#tag EndWindowCode

#tag Events ButtonPing
	#tag Event
		Sub Action()
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendPing()
		    LogMessage(">> PING")
		    pTxCount = pTxCount + 1
		    UpdateStats()
		  Else
		    LogMessage("!! Not connected")
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonStatus
	#tag Event
		Sub Action()
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendStatus()
		    LogMessage(">> STATUS")
		    pTxCount = pTxCount + 1
		    UpdateStats()
		  Else
		    LogMessage("!! Not connected")
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonArm
	#tag Event
		Sub Action()
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    If Window_Main.pSelectedRocketId < 0 Then
		      LogMessage("!! No rocket selected")
		      Return
		    End If
		    Window_Main.pConnection.SendArm(Window_Main.pSelectedRocketId)
		    LogMessage(">> ARM rocket " + Str(Window_Main.pSelectedRocketId))
		    pTxCount = pTxCount + 1
		    UpdateStats()
		  Else
		    LogMessage("!! Not connected")
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonDisarm
	#tag Event
		Sub Action()
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    If Window_Main.pSelectedRocketId < 0 Then
		      LogMessage("!! No rocket selected")
		      Return
		    End If
		    Window_Main.pConnection.SendDisarm(Window_Main.pSelectedRocketId)
		    LogMessage(">> DISARM rocket " + Str(Window_Main.pSelectedRocketId))
		    pTxCount = pTxCount + 1
		    UpdateStats()
		  Else
		    LogMessage("!! Not connected")
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonDownload
	#tag Event
		Sub Action()
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendDownload(Window_Main.pSelectedRocketId)
		    LogMessage(">> DOWNLOAD")
		    pTxCount = pTxCount + 1
		    UpdateStats()
		  Else
		    LogMessage("!! Not connected")
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonReset
	#tag Event
		Sub Action()
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    If Window_Main.pSelectedRocketId < 0 Then
		      LogMessage("!! No rocket selected")
		      Return
		    End If
		    Window_Main.pConnection.SendReset(Window_Main.pSelectedRocketId)
		    LogMessage(">> RESET rocket " + Str(Window_Main.pSelectedRocketId))
		    pTxCount = pTxCount + 1
		    UpdateStats()
		  Else
		    LogMessage("!! Not connected")
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events TextFieldCommand
	#tag Event
		Function KeyDown(Key As String) As Boolean
		  // Send on Enter key
		  If Asc(Key) = 13 Then
		    If TextFieldCommand.Text.Trim <> "" Then
		      SendRawCommand(TextFieldCommand.Text.Trim)
		      TextFieldCommand.Text = ""
		    End If
		    Return True
		  End If
		End Function
	#tag EndEvent
#tag EndEvents
#tag Events ButtonSend
	#tag Event
		Sub Action()
		  If TextFieldCommand.Text.Trim <> "" Then
		    SendRawCommand(TextFieldCommand.Text.Trim)
		    TextFieldCommand.Text = ""
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonClear
	#tag Event
		Sub Action()
		  TextAreaLog.Text = ""
		  pRxCount = 0
		  pTxCount = 0
		  pErrorCount = 0
		  UpdateStats()
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events TimerRefresh
	#tag Event
		Sub Action()
		  // Periodically update connection info
		  UpdateConnectionInfo()
		End Sub
	#tag EndEvent
#tag EndEvents
