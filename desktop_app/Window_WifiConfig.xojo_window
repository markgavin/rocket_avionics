#tag Window
Begin Window Window_WifiConfig
   Backdrop        =   0
   BackgroundColor =   &cFFFFFF00
   Composite       =   False
   DefaultLocation =   2
   Frame           =   0
   FullScreen      =   False
   HasBackgroundColor=   False
   HasCloseButton  =   True
   HasFullScreenButton=   False
   HasMaximizeButton=   False
   HasMinimizeButton=   True
   Height          =   480
   ImplicitInstance=   True
   MacProcID       =   0
   MaximumHeight   =   32000
   MaximumWidth    =   32000
   MenuBar         =   4294967314
   MenuBarVisible  =   True
   MinimumHeight   =   400
   MinimumWidth    =   500
   Resizeable      =   True
   Title           =   "WiFi Configuration"
   Visible         =   True
   Width           =   540
   Begin GroupBox GroupStatus
      AllowAutoDeactivate=   True
      Bold            =   False
      Caption         =   "Current WiFi Status"
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
      LockRight       =   True
      LockTop         =   True
      Scope           =   0
      TabIndex        =   0
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   20
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   500
      Begin Label LabelMode
         AllowAutoDeactivate=   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupStatus"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   False
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Mode: --"
         TextAlignment   =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   45
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   150
      End
      Begin Label LabelSsid
         AllowAutoDeactivate=   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupStatus"
         Italic          =   False
         Left            =   200
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   False
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "SSID: --"
         TextAlignment   =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   45
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   200
      End
      Begin Label LabelIp
         AllowAutoDeactivate=   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupStatus"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   False
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "IP: --"
         TextAlignment   =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   70
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   150
      End
      Begin Label LabelRssi
         AllowAutoDeactivate=   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupStatus"
         Italic          =   False
         Left            =   200
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   False
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Signal: --"
         TextAlignment   =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   70
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   150
      End
      Begin PushButton ButtonRefreshStatus
         AutoDeactivate  =   True
         Bold            =   False
         ButtonStyle     =   0
         Cancel          =   False
         Caption         =   "Refresh"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupStatus"
         Italic          =   False
         Left            =   410
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   False
         LockRight       =   True
         LockTop         =   True
         Scope           =   0
         TabIndex        =   4
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   55
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   80
      End
   End
   Begin GroupBox GroupNetworks
      AllowAutoDeactivate=   True
      Bold            =   False
      Caption         =   "Stored Networks"
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
      LockRight       =   True
      LockTop         =   True
      Scope           =   0
      TabIndex        =   1
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   130
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   500
      Begin Listbox ListNetworks
         AllowAutoDeactivate=   True
         AllowAutoHideScrollbars=   True
         AllowExpandableRows=   False
         AllowFocusRing  =   True
         AllowResizableColumns=   False
         AllowRowDragging=   False
         AllowRowReordering=   False
         Bold            =   False
         ColumnCount     =   3
         ColumnWidths    =   "200,80,80"
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
         Height          =   120
         Index           =   -2147483648
         InitialParent   =   "GroupNetworks"
         InitialValue    =   "SSID	Priority	Enabled"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         RequiresSelection=   False
         RowSelectionType=   0
         Scope           =   0
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   155
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   370
         _ScrollOffset   =   0
         _ScrollWidth    =   -1
      End
      Begin PushButton ButtonRemove
         AutoDeactivate  =   True
         Bold            =   False
         ButtonStyle     =   0
         Cancel          =   False
         Caption         =   "Remove"
         Default         =   False
         Enabled         =   False
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupNetworks"
         Italic          =   False
         Left            =   420
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   False
         LockRight       =   True
         LockTop         =   True
         Scope           =   0
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   155
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   80
      End
      Begin PushButton ButtonRefreshList
         AutoDeactivate  =   True
         Bold            =   False
         ButtonStyle     =   0
         Cancel          =   False
         Caption         =   "Refresh"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupNetworks"
         Italic          =   False
         Left            =   420
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   False
         LockRight       =   True
         LockTop         =   True
         Scope           =   0
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   185
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   80
      End
   End
   Begin GroupBox GroupAdd
      AllowAutoDeactivate=   True
      Bold            =   False
      Caption         =   "Add Network"
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
      LockRight       =   True
      LockTop         =   True
      Scope           =   0
      TabIndex        =   2
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   320
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   500
      Begin Label LabelAddSsid
         AllowAutoDeactivate=   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupAdd"
         Italic          =   False
         Left            =   33
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   False
         TabIndex        =   0
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "SSID:"
         TextAlignment   =   2
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   345
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   50
      End
      Begin TextField FieldSsid
         AllowAutoDeactivate=   True
         AllowFocusRing  =   True
         AllowSpellChecking=   False
         AllowTabs       =   False
         BackgroundColor =   &cFFFFFF00
         Bold            =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Format          =   ""
         HasBorder       =   True
         Height          =   22
         Hint            =   "Network name"
         Index           =   -2147483648
         InitialParent   =   "GroupAdd"
         Italic          =   False
         Left            =   95
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MaximumCharactersAllowed=   32
         Password        =   False
         ReadOnly        =   False
         Scope           =   0
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   ""
         TextAlignment   =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   343
         Transparent     =   False
         Underline       =   False
         ValidationMask  =   ""
         Visible         =   True
         Width           =   170
      End
      Begin PushButton ButtonScan
         AutoDeactivate  =   True
         Bold            =   False
         ButtonStyle     =   0
         Cancel          =   False
         Caption         =   "Scan..."
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   22
         Index           =   -2147483648
         InitialParent   =   "GroupAdd"
         Italic          =   False
         Left            =   292
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Scope           =   0
         TabIndex        =   7
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   "Scan for available WiFi networks"
         Top             =   344
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelAddPassword
         AllowAutoDeactivate=   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupAdd"
         Italic          =   False
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   False
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Password:"
         TextAlignment   =   2
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   378
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   65
      End
      Begin TextField FieldPassword
         AllowAutoDeactivate=   True
         AllowFocusRing  =   True
         AllowSpellChecking=   False
         AllowTabs       =   False
         BackgroundColor =   &cFFFFFF00
         Bold            =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Format          =   ""
         HasBorder       =   True
         Height          =   22
         Hint            =   "WPA2 password"
         Index           =   -2147483648
         InitialParent   =   "GroupAdd"
         Italic          =   False
         Left            =   117
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         MaximumCharactersAllowed=   64
         Password        =   True
         ReadOnly        =   False
         Scope           =   0
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   ""
         TextAlignment   =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   376
         Transparent     =   False
         Underline       =   False
         ValidationMask  =   ""
         Visible         =   True
         Width           =   165
      End
      Begin Label LabelAddPriority
         AllowAutoDeactivate=   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupAdd"
         Italic          =   False
         Left            =   292
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   False
         TabIndex        =   4
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "Priority:"
         TextAlignment   =   2
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   380
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   50
      End
      Begin TextField FieldPriority
         AllowAutoDeactivate=   True
         AllowFocusRing  =   True
         AllowSpellChecking=   False
         AllowTabs       =   False
         BackgroundColor =   &cFFFFFF00
         Bold            =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Format          =   ""
         HasBorder       =   True
         Height          =   22
         Hint            =   "0-255 (0=highest)"
         Index           =   -2147483648
         InitialParent   =   "GroupAdd"
         Italic          =   False
         Left            =   347
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   True
         MaximumCharactersAllowed=   3
         Password        =   False
         ReadOnly        =   False
         Scope           =   0
         TabIndex        =   5
         TabPanelIndex   =   0
         TabStop         =   True
         Text            =   "0"
         TextAlignment   =   0
         TextColor       =   &c00000000
         Tooltip         =   ""
         Top             =   378
         Transparent     =   False
         Underline       =   False
         ValidationMask  =   "###"
         Visible         =   True
         Width           =   60
      End
      Begin PushButton ButtonAdd
         AutoDeactivate  =   True
         Bold            =   False
         ButtonStyle     =   0
         Cancel          =   False
         Caption         =   "Add Network"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "GroupAdd"
         Italic          =   False
         Left            =   404
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   False
         LockRight       =   True
         LockTop         =   True
         Scope           =   0
         TabIndex        =   6
         TabPanelIndex   =   0
         TabStop         =   True
         Tooltip         =   ""
         Top             =   345
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
   End
   Begin PushButton ButtonSave
      AutoDeactivate  =   True
      Bold            =   True
      ButtonStyle     =   0
      Cancel          =   False
      Caption         =   "Save to Flash"
      Default         =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   30
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      Scope           =   0
      TabIndex        =   3
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   "Save WiFi configuration to gateway flash memory"
      Top             =   435
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   120
   End
   Begin PushButton ButtonClose
      AutoDeactivate  =   True
      Bold            =   False
      ButtonStyle     =   0
      Cancel          =   True
      Caption         =   "Close"
      Default         =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   30
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   420
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   False
      Scope           =   0
      TabIndex        =   4
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   435
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   100
   End
   Begin Label LabelInfo
      AllowAutoDeactivate=   True
      Bold            =   False
      DataField       =   ""
      DataSource      =   ""
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   11.0
      FontUnit        =   0
      Height          =   30
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   True
      Left            =   150
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   False
      Multiline       =   True
      Scope           =   0
      Selectable      =   False
      TabIndex        =   5
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "Networks are tried in priority order. Lower priority number = tried first. Changes require Save to persist."
      TextAlignment   =   1
      TextColor       =   &c66666600
      Tooltip         =   ""
      Top             =   435
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   260
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Open()
		  // Request current status and network list when window opens
		  RefreshStatus
		  RefreshNetworkList
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h0
		Sub HandleWifiList(inJson As JSONItem)
		  // Update network list from wifi_list response
		  ListNetworks.RemoveAllRows
		  
		  Var theCount As Integer = inJson.Lookup("count", 0)
		  Var theNetworks As JSONItem = inJson.Lookup("networks", Nil)
		  
		  If theNetworks <> Nil And theNetworks.IsArray Then
		    For i As Integer = 0 To theNetworks.Count - 1
		      Var theNetwork As JSONItem = theNetworks.ChildAt(i)
		      If theNetwork <> Nil Then
		        Var theSsid As String = theNetwork.Lookup("ssid", "")
		        Var thePriority As Integer = theNetwork.Lookup("priority", 0)
		        Var theEnabled As Boolean = theNetwork.Lookup("enabled", True)
		        
		        ListNetworks.AddRow(theSsid, Str(thePriority), If(theEnabled, "Yes", "No"))
		      End If
		    Next
		  End If
		  
		  // Update remove button state
		  ButtonRemove.Enabled = ListNetworks.SelectedRowIndex >= 0
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub HandleWifiScan(inJson As JSONItem)
		  // Handle scan results - show popup menu
		  Var theCount As Integer = inJson.Lookup("count", 0)
		  Var theNetworks As JSONItem = inJson.Lookup("networks", Nil)
		  
		  If theCount = 0 Or theNetworks = Nil Then
		    MessageBox("No WiFi networks found.")
		    Return
		  End If
		  
		  // Build context menu
		  Var theMenu As New DesktopMenuItem
		  
		  For i As Integer = 0 To theNetworks.Count - 1
		    Var theNetwork As JSONItem = theNetworks.ChildAt(i)
		    If theNetwork <> Nil Then
		      Var theSsid As String = theNetwork.Lookup("ssid", "")
		      Var theRssi As Integer = theNetwork.Lookup("rssi", 0)
		      Var theSecurity As String = theNetwork.Lookup("security", "")
		      
		      // Signal strength indicator (using simple chars for compatibility)
		      Var theSignal As String
		      If theRssi >= -50 Then
		        theSignal = "[****]"
		      ElseIf theRssi >= -60 Then
		        theSignal = "[*** ]"
		      ElseIf theRssi >= -70 Then
		        theSignal = "[**  ]"
		      Else
		        theSignal = "[*   ]"
		      End If
		      
		      // Build menu item text
		      Var theText As String = theSignal + " " + theSsid
		      If theSecurity <> "open" Then
		        theText = theText + " (secured)"
		      End If
		      
		      Var theItem As New DesktopMenuItem(theText)
		      theItem.Tag = theSsid
		      theMenu.AddMenu(theItem)
		    End If
		  Next
		  
		  // Show as context menu
		  Var theSelected As DesktopMenuItem = theMenu.PopUp
		  
		  If theSelected <> Nil And theSelected.Tag <> Nil Then
		    FieldSsid.Text = theSelected.Tag.StringValue
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub HandleWifiStatus(inJson As JSONItem)
		  // Update status display from wifi_status response
		  Var theMode As String = inJson.Lookup("mode", "none")
		  Var theSsid As String = inJson.Lookup("ssid", "")
		  Var theIp As String = inJson.Lookup("ip", "")
		  Var theRssi As Integer = inJson.Lookup("rssi", 0)
		  Var theConnected As Boolean = inJson.Lookup("connected", False)
		  
		  // Update labels
		  If theMode = "station" Then
		    LabelMode.Text = "Mode: Station"
		  ElseIf theMode = "ap" Then
		    LabelMode.Text = "Mode: AP"
		  Else
		    LabelMode.Text = "Mode: None"
		  End If
		  
		  LabelSsid.Text = "SSID: " + If(theSsid <> "", theSsid, "--")
		  LabelIp.Text = "IP: " + If(theIp <> "", theIp, "--")
		  
		  If theMode = "station" Then
		    LabelRssi.Text = "Signal: " + Str(theRssi) + " dBm"
		  ElseIf theMode = "ap" Then
		    LabelRssi.Text = "Client: " + If(theConnected, "Connected", "None")
		  Else
		    LabelRssi.Text = "Signal: --"
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub RefreshAfterDelay(sender As Timer)
		  RemoveHandler sender.Action, WeakAddressOf RefreshAfterDelay
		  RefreshNetworkList
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub RefreshNetworkList()
		  // Request network list from gateway
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendRaw("{""cmd"":""wifi_list"",""id"":2}")
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub RefreshStatus()
		  // Request WiFi status from gateway
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendRaw("{""cmd"":""wifi_status"",""id"":1}")
		  End If
		End Sub
	#tag EndMethod


#tag EndWindowCode

#tag Events ButtonRefreshStatus
	#tag Event
		Sub Action()
		  RefreshStatus
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ListNetworks
	#tag Event
		Sub Change()
		  // Enable/disable remove button based on selection
		  ButtonRemove.Enabled = Me.SelectedRowIndex >= 0
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonRemove
	#tag Event
		Sub Action()
		  // Remove selected network
		  Var theIndex As Integer = ListNetworks.SelectedRowIndex
		  If theIndex < 0 Then Return
		  
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendRaw("{""cmd"":""wifi_remove"",""id"":3,""index"":" + Str(theIndex) + "}")
		    
		    // Refresh list after a short delay
		    Var t As New Timer
		    t.Period = 500
		    t.RunMode = Timer.RunModes.Single
		    AddHandler t.Action, WeakAddressOf RefreshAfterDelay
		    t.Enabled = True
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonRefreshList
	#tag Event
		Sub Action()
		  RefreshNetworkList
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonScan
	#tag Event
		Sub Action()
		  // Request WiFi scan from gateway
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Me.Caption = "Scanning..."
		    Me.Enabled = False
		    Window_Main.pConnection.SendRaw("{""cmd"":""wifi_scan"",""id"":10}")
		  Else
		    MessageBox("Not connected to gateway.")
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonAdd
	#tag Event
		Sub Action()
		  // Add new network
		  Var theSsid As String = FieldSsid.Text.Trim
		  Var thePassword As String = FieldPassword.Text
		  Var thePriority As Integer = Val(FieldPriority.Text)
		  
		  If theSsid = "" Then
		    MessageBox("Please enter a network SSID.")
		    Return
		  End If
		  
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Var theJson As String = "{""cmd"":""wifi_add"",""id"":4,""ssid"":""" + theSsid + _
		    """,""password"":""" + thePassword + """,""priority"":" + Str(thePriority) + "}"
		    
		    Window_Main.pConnection.SendRaw(theJson)
		    
		    // Clear fields
		    FieldSsid.Text = ""
		    FieldPassword.Text = ""
		    FieldPriority.Text = "0"
		    
		    // Refresh list after a short delay
		    Var t As New Timer
		    t.Period = 500
		    t.RunMode = Timer.RunModes.Single
		    AddHandler t.Action, WeakAddressOf RefreshAfterDelay
		    t.Enabled = True
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonSave
	#tag Event
		Sub Action()
		  // Save configuration to flash
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendRaw("{""cmd"":""wifi_save"",""id"":5}")
		    MessageBox("WiFi configuration saved to flash.")
		    
		    // Refresh the stored networks list
		    Var t As New Timer
		    t.Period = 500
		    t.RunMode = Timer.RunModes.Single
		    AddHandler t.Action, WeakAddressOf RefreshAfterDelay
		    t.Enabled = True
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonClose
	#tag Event
		Sub Action()
		  Self.Close
		End Sub
	#tag EndEvent
#tag EndEvents
#tag ViewBehavior
	#tag ViewProperty
		Name="Name"
		Visible=true
		Group="ID"
		InitialValue=""
		Type="String"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Interfaces"
		Visible=true
		Group="ID"
		InitialValue=""
		Type="String"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Super"
		Visible=true
		Group="ID"
		InitialValue=""
		Type="String"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Width"
		Visible=true
		Group="Size"
		InitialValue="600"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Height"
		Visible=true
		Group="Size"
		InitialValue="400"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MinimumWidth"
		Visible=true
		Group="Size"
		InitialValue="64"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MinimumHeight"
		Visible=true
		Group="Size"
		InitialValue="64"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MaximumWidth"
		Visible=true
		Group="Size"
		InitialValue="32000"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MaximumHeight"
		Visible=true
		Group="Size"
		InitialValue="32000"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Type"
		Visible=true
		Group="Frame"
		InitialValue="0"
		Type="Types"
		EditorType="Enum"
		#tag EnumValues
			"0 - Document"
			"1 - Movable Modal"
			"2 - Modal Dialog"
			"3 - Floating Window"
			"4 - Plain Box"
			"5 - Shadowed Box"
			"6 - Rounded Window"
			"7 - Global Floating Window"
			"8 - Sheet Window"
			"9 - Metal Window"
			"11 - Modeless Dialog"
		#tag EndEnumValues
	#tag EndViewProperty
	#tag ViewProperty
		Name="Title"
		Visible=true
		Group="Frame"
		InitialValue="Untitled"
		Type="String"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasCloseButton"
		Visible=true
		Group="Frame"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasMaximizeButton"
		Visible=true
		Group="Frame"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasMinimizeButton"
		Visible=true
		Group="Frame"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasFullScreenButton"
		Visible=true
		Group="Frame"
		InitialValue="False"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Resizeable"
		Visible=true
		Group="Frame"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="Composite"
		Visible=false
		Group="OS X (Carbon)"
		InitialValue="False"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MacProcID"
		Visible=false
		Group="OS X (Carbon)"
		InitialValue="0"
		Type="Integer"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="FullScreen"
		Visible=false
		Group="Behavior"
		InitialValue="False"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="ImplicitInstance"
		Visible=true
		Group="Behavior"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="DefaultLocation"
		Visible=true
		Group="Behavior"
		InitialValue="0"
		Type="Locations"
		EditorType="Enum"
		#tag EnumValues
			"0 - Default"
			"1 - Parent Window"
			"2 - Main Screen"
			"3 - Parent Window Screen"
			"4 - Stagger"
		#tag EndEnumValues
	#tag EndViewProperty
	#tag ViewProperty
		Name="Visible"
		Visible=true
		Group="Behavior"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="HasBackgroundColor"
		Visible=true
		Group="Background"
		InitialValue="False"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="BackgroundColor"
		Visible=true
		Group="Background"
		InitialValue="&hFFFFFF"
		Type="Color"
		EditorType="Color"
	#tag EndViewProperty
	#tag ViewProperty
		Name="Backdrop"
		Visible=true
		Group="Background"
		InitialValue=""
		Type="Picture"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MenuBar"
		Visible=true
		Group="Menus"
		InitialValue=""
		Type="MenuBar"
		EditorType=""
	#tag EndViewProperty
	#tag ViewProperty
		Name="MenuBarVisible"
		Visible=true
		Group="Deprecated"
		InitialValue="True"
		Type="Boolean"
		EditorType=""
	#tag EndViewProperty
#tag EndViewBehavior
