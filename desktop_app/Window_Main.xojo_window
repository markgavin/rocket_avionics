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
   Height          =   698
   ImplicitInstance=   True
   LiveResize      =   "True"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   True
   MaxWidth        =   32000
   MenuBar         =   4294967314
   MenuBarVisible  =   True
   MinHeight       =   698
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
      Height          =   150
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
      Begin PopupMenu PopupConnectionType
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         InitialValue    =   "USB Serial\nWiFi"
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
         Top             =   45
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   120
      End
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
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   75
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   240
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
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   75
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   25
      End
      Begin TextField FieldWifiHost
         AllowFocusRing  =   True
         AllowSpellChecking=   False
         AllowTabs       =   False
         AutoDeactivate  =   True
         BackgroundColor =   &cFFFFFF00
         Bold            =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Format          =   ""
         HasBorder       =   True
         Height          =   22
         Hint            =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MaximumCharactersAllowed=   0
         Password        =   False
         ReadOnly        =   False
         Scope           =   2
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "RocketGateway.local"
         TextAlignment   =   0
         TextColor       =   &c00000000
         Tooltip         =   "Gateway hostname or IP address"
         Top             =   75
         Transparent     =   False
         Underline       =   False
         ValidationMask  =   ""
         Visible         =   False
         Width           =   168
      End
      Begin TextField FieldWifiPort
         AllowFocusRing  =   True
         AllowSpellChecking=   False
         AllowTabs       =   False
         AutoDeactivate  =   True
         BackgroundColor =   &cFFFFFF00
         Bold            =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Format          =   ""
         HasBorder       =   True
         Height          =   22
         Hint            =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupConnection"
         Italic          =   False
         Left            =   220
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MaximumCharactersAllowed=   0
         Password        =   False
         ReadOnly        =   False
         Scope           =   2
         TabIndex        =   4
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "5000"
         TextAlignment   =   0
         TextColor       =   &c00000000
         Tooltip         =   "Gateway port"
         Top             =   74
         Transparent     =   False
         Underline       =   False
         ValidationMask  =   ""
         Visible         =   False
         Width           =   60
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
         Left            =   172
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
         Top             =   45
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   108
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
         TabIndex        =   6
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Disconnected"
         TextAlign       =   0
         TextColor       =   &c99999900
         Tooltip         =   ""
         Top             =   105
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   200
      End
      Begin Label LabelWifiInfo
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
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
         TabIndex        =   7
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Connect to 'RocketGateway' WiFi first"
         TextAlign       =   0
         TextColor       =   &c66666600
         TextSize        =   10.0
         Tooltip         =   ""
         Top             =   125
         Transparent     =   False
         Underline       =   False
         Visible         =   False
         Width           =   240
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
      Height          =   274
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Scope           =   2
      TabIndex        =   1
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   180
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   280
      Begin Listbox ListRockets
         AllowAutoDeactivate=   True
         AllowAutoHideScrollbars=   True
         AllowExpandableRows=   False
         AllowFocusRing  =   True
         AllowResizableColumns=   False
         AllowRowDragging=   False
         AllowRowReordering=   False
         Bold            =   False
         ColumnCount     =   2
         ColumnWidths    =   "*,50"
         DefaultRowHeight=   -1
         DropIndicatorVisible=   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         GridLinesHorizontalStyle=   0
         GridLinesVerticalStyle=   0
         HasBorder       =   True
         HasHeader       =   True
         HasHorizontalScrollbar=   False
         HasVerticalScrollbar=   True
         HeadingIndex    =   -1
         Height          =   95
         Index           =   -2147483648
         InitialParent   =   "GroupTelemetry"
         InitialValue    =   "Name	ID"
         Italic          =   False
         Left            =   40
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         RequiresSelection=   False
         RowSelectionType=   0
         Scope           =   2
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   "Select rocket to display"
         Top             =   216
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   240
         _ScrollOffset   =   0
         _ScrollWidth    =   -1
      End
      Begin Label LabelAltitude
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
         Text            =   "Altitude: --- m"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   16.0
         Tooltip         =   ""
         Top             =   323
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
         Height          =   22
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
         Text            =   "Velocity: --- m/s"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextSize        =   14.0
         Tooltip         =   ""
         Top             =   348
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
         Height          =   22
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
         Text            =   "State: ---"
         TextAlign       =   0
         TextColor       =   &c00660000
         TextSize        =   14.0
         Tooltip         =   ""
         Top             =   370
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
         TabIndex        =   4
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "RSSI: --- dBm  SNR: --- dB"
         TextAlign       =   0
         TextColor       =   &c66666600
         TextSize        =   11.0
         Tooltip         =   ""
         Top             =   392
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   240
      End
      Begin Label LabelDistance
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
         TabIndex        =   5
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Distance: --- m  Bearing: ---°"
         TextAlign       =   0
         TextColor       =   &c66666600
         TextSize        =   11.0
         Tooltip         =   ""
         Top             =   414
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   240
      End
   End
   Begin GroupBox GroupGatewayGps
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Gateway GPS"
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   70
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      Scope           =   2
      TabIndex        =   4
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   466
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   280
      Begin Label LabelGatewayGpsStatus
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
         InitialParent   =   "GroupGatewayGps"
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
         Text            =   "Status: ---"
         TextAlign       =   0
         TextColor       =   &c66666600
         Tooltip         =   ""
         Top             =   488
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   240
      End
      Begin Label LabelGatewayGpsCoords
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
         InitialParent   =   "GroupGatewayGps"
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
         Text            =   "Lat: ---  Lon: ---"
         TextAlign       =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   508
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
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      Scope           =   2
      TabIndex        =   2
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   548
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
         Top             =   583
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
         Top             =   583
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
         Top             =   633
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
      Height          =   648
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
		  
		  // Save WiFi preferences
		  Module_Preferences.ConnectionType = PopupConnectionType.SelectedRowIndex
		  Module_Preferences.WifiHost = FieldWifiHost.Text
		  Module_Preferences.WifiPort = Val(FieldWifiPort.Text)
		  Module_Preferences.SavePreferences()
		  
		  // Clean up
		  If pConnection <> Nil Then
		    RemoveHandler pConnection.ConnectionChanged, AddressOf HandleConnectionChanged
		    RemoveHandler pConnection.TelemetryReceived, AddressOf HandleTelemetryReceived
		    RemoveHandler pConnection.LinkStatusChanged, AddressOf HandleLinkStatusChanged
		    RemoveHandler pConnection.ErrorReceived, AddressOf HandleErrorReceived
		    RemoveHandler pConnection.StorageListReceived, AddressOf HandleStorageListReceived
		    RemoveHandler pConnection.StorageDataReceived, AddressOf HandleStorageDataReceived
		    RemoveHandler pConnection.DeviceInfoReceived, AddressOf HandleDeviceInfoReceived
		    RemoveHandler pConnection.GatewaySettingsReceived, AddressOf HandleGatewaySettingsReceived
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
		  AddHandler pConnection.GatewaySettingsReceived, AddressOf HandleGatewaySettingsReceived
		  
		  // Initialize current flight data
		  pCurrentFlight = New FlightData
		  
		  // Refresh serial ports
		  RefreshSerialPorts()
		  
		  // Load WiFi preferences
		  PopupConnectionType.ListIndex = Module_Preferences.ConnectionType
		  FieldWifiHost.Text = Module_Preferences.WifiHost
		  FieldWifiPort.Text = Str(Module_Preferences.WifiPort)
		  
		  // Update connection type controls visibility
		  UpdateConnectionTypeVisibility()
		  
		  // Apply dark mode appearance
		  ApplyAppearance()
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h21
		Private Sub ApplyAppearance()
		  // Check if dark mode is enabled (macOS only)
		  #If TargetMacOS Then
		    pIsDarkMode = Color.IsDarkMode
		  #Else
		    pIsDarkMode = False
		  #EndIf
		  
		  // Update chart for dark mode
		  ChartAltitude.pDarkMode = pIsDarkMode
		  ChartAltitude.Refresh
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
		  If inConnected Then
		    // Show connection type in status
		    If inSender.GetConnectionMode = FlightConnection.ConnectionMode.WiFi Then
		      LabelConnectionStatus.Text = "Connected (WiFi)"
		    Else
		      LabelConnectionStatus.Text = "Connected (USB)"
		    End If
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
		  
		  // Display device info in separate window instances (so both can be viewed at once)
		  Try
		    If inIsGateway Then
		      // Create or reuse gateway info window
		      If pGatewayInfoWindow = Nil Then
		        pGatewayInfoWindow = New Window_GatewayInfo
		      End If
		      pGatewayInfoWindow.ShowGatewayInfo(inInfo)
		      
		      // Also update gateway GPS labels on main window
		      Var theGpsFix As Boolean = inInfo.Lookup("gps_fix", False)
		      If theGpsFix Then
		        LabelGatewayGpsStatus.Text = "Status: Fix (" + Str(inInfo.Lookup("gps_sats", 0)) + " sats)"
		        LabelGatewayGpsStatus.TextColor = &c00AA00
		        LabelGatewayGpsCoords.Text = "Lat: " + Format(inInfo.Lookup("gps_lat", 0.0), "0.00000") + _
		        "  Lon: " + Format(inInfo.Lookup("gps_lon", 0.0), "0.00000")
		      Else
		        Var theGpsOk As Boolean = inInfo.Lookup("gps", False)
		        If theGpsOk Then
		          LabelGatewayGpsStatus.Text = "Status: No Fix (acquiring...)"
		        Else
		          LabelGatewayGpsStatus.Text = "Status: GPS Not Found"
		        End If
		        LabelGatewayGpsStatus.TextColor = &c99999900
		        LabelGatewayGpsCoords.Text = "Lat: ---  Lon: ---"
		      End If
		    Else
		      // Create or reuse flight computer info window
		      If pFlightComputerInfoWindow = Nil Then
		        pFlightComputerInfoWindow = New Window_DeviceInfo
		      End If
		      pFlightComputerInfoWindow.ShowFlightComputerInfo(inInfo)
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
		Private Sub HandleGatewaySettingsReceived(inSender As FlightConnection, inBacklight As Integer, inBacklightMax As Integer, inLoraTxPower As Integer, inLoraTxMin As Integer, inLoraTxMax As Integer, inWifiTxPower As Integer, inWifiTxMin As Integer, inWifiTxMax As Integer)
		  #Pragma Unused inSender
		  #Pragma Unused inBacklight
		  #Pragma Unused inBacklightMax
		  #Pragma Unused inLoraTxPower
		  #Pragma Unused inLoraTxMin
		  #Pragma Unused inLoraTxMax
		  #Pragma Unused inWifiTxPower
		  #Pragma Unused inWifiTxMin
		  #Pragma Unused inWifiTxMax
		  
		  // TODO: Gateway settings dialog - re-add via Xojo IDE
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
		  
		  // Track this rocket as active
		  UpdateActiveRocket(inSample.pRocketId)
		  
		  // Only update display if this is the selected rocket (or auto-select if none selected)
		  If pSelectedRocketId < 0 Then
		    // Auto-select first rocket
		    pSelectedRocketId = inSample.pRocketId
		    UpdateRocketListSelection
		  ElseIf inSample.pRocketId <> pSelectedRocketId Then
		    // Not the selected rocket - don't update display
		    Return
		  End If
		  
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
		  
		  // Show "ORIENTATION" when in orientation mode and idle state
		  If inSample.IsOrientationMode And inSample.pState = "idle" Then
		    LabelState.Text = "State: ORIENTATION"
		    LabelState.TextColor = &c9900CC  // Purple for orientation mode
		  Else
		    LabelState.Text = "State: " + inSample.pState.Uppercase
		    
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
		  End If
		  
		  // Show RSSI/SNR and ground station altitude (if available)
		  Var theRssiText As String = "RSSI: " + Str(inSample.pRssi) + " dBm  SNR: " + Str(inSample.pSnr) + " dB"
		  If inSample.pGroundAltitudeM <> 0.0 Then
		    theRssiText = theRssiText + "  Gnd: " + Format(inSample.pGroundAltitudeM, "0") + " m"
		  End If
		  LabelRssi.Text = theRssiText
		  
		  // Show distance and bearing from gateway to flight computer (if both have GPS fix)
		  If inSample.pGpsFix And inSample.pGatewayGpsFix Then
		    Var theDistM As Double = inSample.DistanceToFlightM
		    Var theBearing As Double = inSample.BearingToFlightDeg
		    Var theDistText As String
		    If theDistM >= 1000 Then
		      theDistText = Format(theDistM / 1000.0, "0.00") + " km"
		    Else
		      theDistText = Format(theDistM, "0") + " m"
		    End If
		    LabelDistance.Text = "Distance: " + theDistText + "  Bearing: " + Format(theBearing, "0") + Chr(176)
		    LabelDistance.TextColor = &c006600
		  Else
		    If Not inSample.pGpsFix And Not inSample.pGatewayGpsFix Then
		      LabelDistance.Text = "Distance: --- (no GPS fix)"
		    ElseIf Not inSample.pGpsFix Then
		      LabelDistance.Text = "Distance: --- (flight GPS no fix)"
		    Else
		      LabelDistance.Text = "Distance: --- (gateway GPS no fix)"
		    End If
		    LabelDistance.TextColor = &c99999900
		  End If
		  
		  // Update gateway GPS display
		  If inSample.pGatewayGpsFix Then
		    LabelGatewayGpsStatus.Text = "Status: Fix"
		    LabelGatewayGpsStatus.TextColor = &c00AA00
		    LabelGatewayGpsCoords.Text = "Lat: " + Format(inSample.pGatewayLatitude, "0.00000") + _
		    "  Lon: " + Format(inSample.pGatewayLongitude, "0.00000")
		  Else
		    LabelGatewayGpsStatus.Text = "Status: No Fix"
		    LabelGatewayGpsStatus.TextColor = &c99999900
		    LabelGatewayGpsCoords.Text = "Lat: ---  Lon: ---"
		  End If
		  
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
		  
		  // Forward telemetry to orientation window if active
		  // Use shared flag to avoid implicitly creating the window
		  If Window_Orientation.sOrientationModeActive Then
		    Window_Orientation.UpdateFromTelemetry(inSample)
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub RefreshRocketList()
		  // Update the rocket listbox with active rockets
		  If pActiveRockets = Nil Then Return
		  
		  pIsRefreshingRocketList = True
		  
		  // Save current selection
		  Var theSavedSelection As Integer = pSelectedRocketId
		  
		  ListRockets.RemoveAllRows
		  
		  // Sort rocket IDs
		  Var theIds() As Integer
		  For Each theKey As Variant In pActiveRockets.Keys
		    theIds.Add(theKey.IntegerValue)
		  Next
		  theIds.Sort
		  
		  // Add rockets to list (Name, ID columns)
		  For Each theId As Integer In theIds
		    Var theName As String = Module_Preferences.GetStringPref("RocketName_" + Str(theId), "")
		    If theName = "" Then
		      theName = "Rocket #" + Str(theId)
		    End If
		    ListRockets.AddRow(theName, Str(theId))
		    ListRockets.RowTagAt(ListRockets.LastAddedRowIndex) = theId
		  Next
		  
		  // Restore selection
		  pSelectedRocketId = theSavedSelection
		  UpdateRocketListSelection
		  
		  pIsRefreshingRocketList = False
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
		  LabelDistance.Text = "Distance: --- m  Bearing: ---" + Chr(176)
		  
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

	#tag Method, Flags = &h21
		Private Sub UpdateActiveRocket(inRocketId As Integer)
		  // Track this rocket as active with current timestamp
		  If pActiveRockets = Nil Then
		    pActiveRockets = New Dictionary
		  End If
		  
		  Var theNow As Double = Microseconds / 1000000.0
		  Var theWasKnown As Boolean = pActiveRockets.HasKey(inRocketId)
		  pActiveRockets.Value(inRocketId) = theNow
		  
		  // Update the rocket list if this is a new rocket
		  If Not theWasKnown Then
		    RefreshRocketList
		  End If
		  
		  // Clean up stale rockets (not seen in 10 seconds)
		  Var theStaleIds() As Integer
		  For Each theKey As Variant In pActiveRockets.Keys
		    Var theId As Integer = theKey.IntegerValue
		    Var theLastSeen As Double = pActiveRockets.Value(theId).DoubleValue
		    If (theNow - theLastSeen) > 10.0 Then
		      theStaleIds.Add(theId)
		    End If
		  Next
		  
		  For Each theId As Integer In theStaleIds
		    pActiveRockets.Remove(theId)
		  Next
		  
		  If theStaleIds.Count > 0 Then
		    RefreshRocketList
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub UpdateConnectionTypeVisibility()
		  // Show/hide appropriate controls based on connection type
		  Var theIsWifi As Boolean = (PopupConnectionType.SelectedRowIndex = 1)
		  
		  // Show serial controls when USB Serial selected
		  PopupSerialPort.Visible = Not theIsWifi
		  ButtonRefreshPorts.Visible = Not theIsWifi
		  
		  // Show WiFi controls when WiFi selected
		  FieldWifiHost.Visible = theIsWifi
		  FieldWifiPort.Visible = theIsWifi
		  LabelWifiInfo.Visible = theIsWifi
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub UpdateRocketListSelection()
		  // Select the row matching pSelectedRocketId
		  If pSelectedRocketId < 0 Then Return
		  
		  For i As Integer = 0 To ListRockets.RowCount - 1
		    If ListRockets.RowTagAt(i).IntegerValue = pSelectedRocketId Then
		      If ListRockets.SelectedRowIndex <> i Then
		        pIsRefreshingRocketList = True
		        ListRockets.SelectedRowIndex = i
		        pIsRefreshingRocketList = False
		      End If
		      Return
		    End If
		  Next
		  
		  // Selected rocket not in list - select first if available
		  If ListRockets.RowCount > 0 Then
		    pSelectedRocketId = ListRockets.RowTagAt(0).IntegerValue
		    pIsRefreshingRocketList = True
		    ListRockets.SelectedRowIndex = 0
		    pIsRefreshingRocketList = False
		  Else
		    pSelectedRocketId = -1
		  End If
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pActiveRockets As Dictionary
	#tag EndProperty

	#tag Property, Flags = &h0
		pConnection As FlightConnection
	#tag EndProperty

	#tag Property, Flags = &h0
		pCurrentFlight As FlightData
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pFlightComputerInfoWindow As Window_DeviceInfo
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pFlightSaved As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pGatewayInfoWindow As Window_GatewayInfo
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pIsDarkMode As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pIsRefreshingRocketList As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pPreviousState As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		pSelectedRocketId As Integer = -1
	#tag EndProperty


#tag EndWindowCode

#tag Events PopupConnectionType
	#tag Event
		Sub Change()
		  UpdateConnectionTypeVisibility()
		End Sub
	#tag EndEvent
#tag EndEvents
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
		    // Check connection type
		    If PopupConnectionType.SelectedRowIndex = 1 Then
		      // WiFi connection
		      Var theHost As String = FieldWifiHost.Text.Trim
		      Var thePort As Integer = Val(FieldWifiPort.Text)
		      
		      If theHost = "" Then
		        MessageBox("Please enter a gateway IP address.")
		        Return
		      End If
		      
		      If thePort <= 0 Or thePort > 65535 Then
		        MessageBox("Please enter a valid port number (1-65535).")
		        Return
		      End If
		      
		      Call pConnection.ConnectWifi(theHost, thePort)
		    Else
		      // USB Serial connection
		      If PopupSerialPort.SelectedRowIndex >= 0 Then
		        Var thePort As String = PopupSerialPort.Text
		        If thePort <> "(No ports found)" Then
		          Call pConnection.Connect(thePort)
		        End If
		      End If
		    End If
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ListRockets
	#tag Event
		Sub Change()
		  // Ignore changes during refresh
		  If pIsRefreshingRocketList Then Return
		  
		  // Update selected rocket
		  If Me.SelectedRowIndex >= 0 Then
		    pSelectedRocketId = Me.RowTagAt(Me.SelectedRowIndex).IntegerValue
		  Else
		    pSelectedRocketId = -1
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonArm
	#tag Event
		Sub Action()
		  If pConnection.IsConnected Then
		    If pSelectedRocketId < 0 Then
		      MessageBox("No rocket selected. Select a rocket first.")
		      Return
		    End If
		    // Confirm arming
		    Var theResult As Integer = MsgBox("ARM rocket " + Str(pSelectedRocketId) + "?" + EndOfLine + EndOfLine + _
		    "This will put the rocket in armed mode, ready for launch detection.", 1, "Confirm Arm")
		    If theResult = 1 Then
		      pConnection.SendArm(pSelectedRocketId)
		    End If
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonDisarm
	#tag Event
		Sub Action()
		  If pConnection.IsConnected And pSelectedRocketId >= 0 Then
		    pConnection.SendDisarm(pSelectedRocketId)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonDownload
	#tag Event
		Sub Action()
		  If pConnection.IsConnected Then
		    pConnection.SendDownload(pSelectedRocketId)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
