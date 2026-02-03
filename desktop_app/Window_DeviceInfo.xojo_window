#tag Window
Begin Window Window_DeviceInfo
   BackColor       =   &cFFFFFF00
   Backdrop        =   0
   CloseButton     =   True
   Composite       =   False
   Frame           =   0
   FullScreen      =   False
   HasBackColor    =   False
   HasFullScreenButton=   False
   Height          =   580
   ImplicitInstance=   True
   LiveResize      =   "False"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   False
   MaxWidth        =   32000
   MenuBar         =   4294967314
   MenuBarVisible  =   True
   MinHeight       =   580
   MinimizeButton  =   True
   MinWidth        =   450
   Placement       =   0
   Resizeable      =   True
   Title           =   "Flight Computer Information"
   Visible         =   False
   Width           =   450
   Begin GroupBox GroupRockets
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Connected Flight Computers"
      Enabled         =   True
      Height          =   130
      HelpTag         =   ""
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
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   14
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   410
      Begin Listbox ListRockets
         AllowAutoDeactivate=   True
         AllowAutoHideScrollbars=   True
         AllowExpandableRows=   False
         AllowFocusRing  =   True
         AllowResizableColumns=   False
         AllowRowDragging=   False
         AllowRowReordering=   False
         Bold            =   False
         ColumnCount     =   3
         ColumnWidths    =   "*,80,60"
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
         Height          =   80
         Index           =   -2147483648
         InitialParent   =   "GroupRockets"
         InitialValue    =   "Name	Status	Signal"
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
         Top             =   38
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   290
         _ScrollOffset   =   0
         _ScrollWidth    =   -1
      End
      Begin PushButton ButtonRename
         AutoDeactivate  =   True
         Bold            =   False
         ButtonStyle     =   0
         Cancel          =   False
         Caption         =   "Rename..."
         Default         =   False
         Enabled         =   False
         Height          =   22
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupRockets"
         Italic          =   False
         Left            =   340
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   False
         LockRight       =   True
         LockTop         =   True
         Scope           =   0
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   True
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   38
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
         Height          =   22
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupRockets"
         Italic          =   False
         Left            =   340
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   False
         LockRight       =   True
         LockTop         =   True
         Scope           =   0
         TabIndex        =   2
         TabPanelIndex   =   0
         TabStop         =   True
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   68
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelNoRockets
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupRockets"
         Italic          =   True
         Left            =   40
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   False
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "No flight computers connected"
         TextAlign       =   1
         TextColor       =   &c99999900
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   118
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   290
      End
   End
   Begin GroupBox GroupFirmware
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Firmware"
      Enabled         =   True
      Height          =   100
      HelpTag         =   ""
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
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   154
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   410
      Begin Label LabelDeviceType
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupFirmware"
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
         TabStop         =   False
         Text            =   "Device:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   178
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelDeviceValue
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupFirmware"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   178
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
      Begin Label LabelVersionType
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupFirmware"
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
         TabStop         =   False
         Text            =   "Version:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   200
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelVersionValue
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupFirmware"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   200
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
      Begin Label LabelBuildType
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupFirmware"
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
         TabIndex        =   4
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "Build:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   222
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelBuildValue
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupFirmware"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   5
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   222
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
   End
   Begin GroupBox GroupHardware
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Hardware Status"
      Enabled         =   True
      Height          =   144
      HelpTag         =   ""
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
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   264
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   410
      Begin Label LabelHw1Type
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
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
         TabStop         =   False
         Text            =   "LoRa Radio:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   288
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelHw1Value
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   288
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
      Begin Label LabelHw2Type
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
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
         TabStop         =   False
         Text            =   "Barometer:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   310
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelHw2Value
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   310
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
      Begin Label LabelHw3Type
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
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
         TabIndex        =   4
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "Display:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   332
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelHw3Value
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   5
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   332
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
      Begin Label LabelHw4Type
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
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
         TabIndex        =   6
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "IMU:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   354
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelHw4Value
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   7
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   354
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
      Begin Label LabelHw5Type
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
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
         TabIndex        =   8
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "GPS:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   376
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelHw5Value
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupHardware"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   9
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   376
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
   End
   Begin GroupBox GroupStatus
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Flight Status"
      Enabled         =   True
      Height          =   100
      HelpTag         =   ""
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
      TabIndex        =   3
      TabPanelIndex   =   0
      TabStop         =   True
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   418
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   410
      Begin Label LabelStat1Type
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
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
         TabStop         =   False
         Text            =   "State:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   442
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelStat1Value
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupStatus"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   1
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   442
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
      Begin Label LabelStat2Type
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
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
         TabStop         =   False
         Text            =   "Altitude:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   464
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelStat2Value
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupStatus"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   3
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   464
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
      Begin Label LabelStat3Type
         AutoDeactivate  =   True
         Bold            =   False
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
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
         TabIndex        =   4
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "GPS Sats:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   486
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelStat3Value
         AutoDeactivate  =   True
         Bold            =   True
         DataField       =   ""
         DataSource      =   ""
         Enabled         =   True
         Height          =   20
         HelpTag         =   ""
         Index           =   -2147483648
         InitialParent   =   "GroupStatus"
         Italic          =   False
         Left            =   150
         LockBottom      =   False
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         Multiline       =   False
         Scope           =   0
         Selectable      =   True
         TabIndex        =   5
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   486
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
   End
   Begin PushButton ButtonClose
      AutoDeactivate  =   True
      Bold            =   False
      ButtonStyle     =   0
      Cancel          =   True
      Caption         =   "Close"
      Default         =   True
      Enabled         =   True
      Height          =   22
      HelpTag         =   ""
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   340
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   False
      Scope           =   0
      TabIndex        =   4
      TabPanelIndex   =   0
      TabStop         =   True
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   538
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   90
   End
   Begin Label LabelStatusMsg
      AutoDeactivate  =   True
      Bold            =   False
      DataField       =   ""
      DataSource      =   ""
      Enabled         =   True
      Height          =   20
      HelpTag         =   ""
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   True
      Left            =   20
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   False
      Multiline       =   False
      Scope           =   0
      Selectable      =   False
      TabIndex        =   5
      TabPanelIndex   =   0
      TabStop         =   False
      Text            =   ""
      TextAlign       =   0
      TextColor       =   &c66666600
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   540
      Transparent     =   True
      Underline       =   False
      Visible         =   True
      Width           =   300
   End
   Begin Timer TimerRefresh
      Enabled         =   False
      Index           =   -2147483648
      LockedInPosition=   False
      Period          =   3000
      RunMode         =   2
      Scope           =   0
      TabPanelIndex   =   0
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Close()
		  // Stop refresh timer
		  TimerRefresh.Enabled = False
		  
		  // Save window position
		  Module_WindowSettings.SaveWindowPosition(Self, "Window_DeviceInfo")
		End Sub
	#tag EndEvent

	#tag Event
		Sub Open()
		  // Restore window position
		  Module_WindowSettings.LoadWindowPosition(Self, "Window_DeviceInfo")
		  
		  // Clear detail panes
		  ClearDetails
		  
		  // Request rocket list
		  RefreshRocketList
		  
		  // Start auto-refresh timer
		  TimerRefresh.Enabled = True
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h21
		Private Sub ClearDetails()
		  // Clear all detail fields
		  LabelDeviceValue.Text = "--"
		  LabelVersionValue.Text = "--"
		  LabelBuildValue.Text = "--"
		  
		  LabelHw1Value.Text = "--"
		  LabelHw1Value.TextColor = &c000000
		  LabelHw2Value.Text = "--"
		  LabelHw2Value.TextColor = &c000000
		  LabelHw3Value.Text = "--"
		  LabelHw3Value.TextColor = &c000000
		  LabelHw4Value.Text = "--"
		  LabelHw4Value.TextColor = &c000000
		  LabelHw5Value.Text = "--"
		  LabelHw5Value.TextColor = &c000000
		  
		  LabelStat1Value.Text = "--"
		  LabelStat2Value.Text = "--"
		  LabelStat3Value.Text = "--"
		  
		  LabelStatusMsg.Text = "Select a flight computer above"
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetRocketName(inRocketId As Integer) As String
		  // Get custom name for rocket from preferences
		  Var theKey As String = "RocketName_" + Str(inRocketId)
		  Return Module_Preferences.GetStringPref(theKey, "")
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub HandleRocketList(inRockets() As Dictionary)
		  // Update the rocket list display
		  pRocketData = inRockets
		  
		  // Save selected rocket ID before clearing (RemoveAllRows triggers Change event)
		  Var theSavedSelection As Integer = pSelectedRocketId
		  
		  // Prevent Change event from clearing selection during refresh
		  pIsRefreshing = True
		  ListRockets.RemoveAllRows
		  pIsRefreshing = False
		  
		  If inRockets.Count = 0 Then
		    LabelNoRockets.Visible = True
		    ButtonRename.Enabled = False
		    ClearDetails
		    pSelectedRocketId = -1
		    Return
		  End If
		  
		  LabelNoRockets.Visible = False
		  
		  For Each theRocket As Dictionary In inRockets
		    Var theId As Integer = theRocket.Lookup("id", 0).IntegerValue
		    Var theState As String = theRocket.Lookup("state", "unknown").StringValue
		    Var theRssi As Integer = theRocket.Lookup("rssi", 0).IntegerValue
		    
		    // Get custom name or default
		    Var theName As String = GetRocketName(theId)
		    If theName = "" Then
		      theName = "Rocket #" + Str(theId)
		    End If
		    
		    // Format state nicely
		    theState = theState.Titlecase
		    
		    // Format signal
		    Var theSignal As String = Str(theRssi) + " dBm"
		    
		    ListRockets.AddRow(theName, theState, theSignal)
		    ListRockets.RowTagAt(ListRockets.LastAddedRowIndex) = theId
		  Next
		  
		  // Re-select previously selected rocket if still in list
		  pSelectedRocketId = theSavedSelection
		  If pSelectedRocketId >= 0 Then
		    pIsRefreshing = True
		    For i As Integer = 0 To ListRockets.RowCount - 1
		      If ListRockets.RowTagAt(i).IntegerValue = pSelectedRocketId Then
		        ListRockets.SelectedRowIndex = i
		        // Update the details with fresh data
		        ShowRocketFromListData(inRockets(i))
		        Exit
		      End If
		    Next
		    pIsRefreshing = False
		  End If
		  
		  ButtonRename.Enabled = ListRockets.SelectedRowIndex >= 0
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub RefreshRocketList()
		  // Request list of connected rockets from gateway
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendCommand("rockets")
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub RequestRocketInfo(inRocketId As Integer)
		  // Show info for the selected rocket from cached data
		  pSelectedRocketId = inRocketId
		  
		  // Find rocket data from cached list
		  For Each theRocket As Dictionary In pRocketData
		    If theRocket.Lookup("id", -1).IntegerValue = inRocketId Then
		      ShowRocketFromListData(theRocket)
		      
		      // Also request detailed fc_info (firmware version, etc.)
		      If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		        Window_Main.pConnection.SendCommand("fc_info")
		      End If
		      Return
		    End If
		  Next
		  
		  ClearDetails
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SetRocketName(inRocketId As Integer, inName As String)
		  // Save custom name for rocket to preferences
		  Var theKey As String = "RocketName_" + Str(inRocketId)
		  Module_Preferences.SetStringPref(theKey, inName)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub ShowFlightComputerInfo(inInfo As Dictionary)
		  // Update firmware details from fc_info response
		  // This gets called when detailed flight computer info is received via LoRa

		  Var theVersion As String = inInfo.Lookup("version", "").StringValue
		  Var theBuild As String = inInfo.Lookup("build", "").StringValue

		  If theVersion <> "" Then
		    LabelVersionValue.Text = "v" + theVersion
		  End If
		  If theBuild <> "" Then
		    LabelBuildValue.Text = theBuild
		  End If

		  // Hardware section - display sensor types
		  LabelHw1Type.Text = "LoRa:"
		  Var hasLora As Boolean = inInfo.Lookup("lora", False).BooleanValue
		  LabelHw1Value.Text = If(hasLora, "OK", "Not detected")
		  LabelHw1Value.TextColor = If(hasLora, &c006600, &cCC0000)

		  LabelHw2Type.Text = "Barometer:"
		  Var baroType As String = inInfo.Lookup("baro_type", "").StringValue
		  Var hasBaro As Boolean = inInfo.Lookup("bmp390", False).BooleanValue
		  If baroType <> "" Then
		    LabelHw2Value.Text = baroType
		  ElseIf hasBaro Then
		    LabelHw2Value.Text = "BMP390"
		  Else
		    LabelHw2Value.Text = "Not detected"
		  End If
		  LabelHw2Value.TextColor = If(hasBaro Or baroType <> "", &c006600, &cCC0000)

		  LabelHw3Type.Text = "IMU:"
		  Var imuType As String = inInfo.Lookup("imu_type", "").StringValue
		  Var hasImu As Boolean = inInfo.Lookup("imu", False).BooleanValue
		  If imuType <> "" Then
		    LabelHw3Value.Text = imuType
		  ElseIf hasImu Then
		    LabelHw3Value.Text = "LSM6DSOX"
		  Else
		    LabelHw3Value.Text = "Not detected"
		  End If
		  LabelHw3Value.TextColor = If(hasImu Or imuType <> "", &c006600, &cCC0000)

		  LabelHw4Type.Text = "GPS:"
		  Var hasGps As Boolean = inInfo.Lookup("gps", False).BooleanValue
		  LabelHw4Value.Text = If(hasGps, "OK", "Not detected")
		  LabelHw4Value.TextColor = If(hasGps, &c006600, &cCC0000)

		  LabelHw5Type.Text = "Display:"
		  Var hasOled As Boolean = inInfo.Lookup("oled", False).BooleanValue
		  LabelHw5Value.Text = If(hasOled, "OK", "Not detected")
		  LabelHw5Value.TextColor = If(hasOled, &c006600, &cCC0000)

		  LabelStatusMsg.Text = "Firmware info received"
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub ShowRocketFromListData(inRocket As Dictionary)
		  // Display rocket details from the rockets list data
		  Var theId As Integer = inRocket.Lookup("id", -1).IntegerValue
		  Var theName As String = GetRocketName(theId)
		  If theName = "" Then
		    theName = "Rocket #" + Str(theId)
		  End If
		  
		  // Firmware section - show rocket name and ID
		  LabelDeviceValue.Text = theName + " (ID: " + Str(theId) + ")"
		  LabelVersionValue.Text = "(Awaiting LoRa response)"
		  LabelBuildValue.Text = "--"
		  
		  // Hardware section - repurpose for rocket telemetry
		  LabelHw1Type.Text = "State:"
		  Var theState As String = inRocket.Lookup("state", "unknown").StringValue
		  LabelHw1Value.Text = theState.Titlecase
		  LabelHw1Value.TextColor = If(theState = "ARMED" Or theState = "Armed", &cCC6600, &c000000)
		  
		  LabelHw2Type.Text = "Signal:"
		  Var theRssi As Integer = inRocket.Lookup("rssi", 0).IntegerValue
		  LabelHw2Value.Text = Str(theRssi) + " dBm"
		  If theRssi > -70 Then
		    LabelHw2Value.TextColor = &c006600
		  ElseIf theRssi > -90 Then
		    LabelHw2Value.TextColor = &c666600
		  Else
		    LabelHw2Value.TextColor = &cCC0000
		  End If
		  
		  LabelHw3Type.Text = "Last Update:"
		  Var theAge As Integer = inRocket.Lookup("age", 0).IntegerValue
		  LabelHw3Value.Text = Str(theAge) + " seconds ago"
		  LabelHw3Value.TextColor = If(theAge < 10, &c006600, &c666600)
		  
		  LabelHw4Type.Text = "GPS Satellites:"
		  Var theSats As Integer = inRocket.Lookup("sats", 0).IntegerValue
		  LabelHw4Value.Text = Str(theSats)
		  LabelHw4Value.TextColor = If(theSats >= 4, &c006600, &c666600)
		  
		  LabelHw5Type.Text = "Distance:"
		  Var theDist As Double = inRocket.Lookup("dist", 0.0).DoubleValue
		  If theDist > 0 Then
		    LabelHw5Value.Text = Format(theDist, "0.0") + " m"
		  Else
		    LabelHw5Value.Text = "Unknown"
		  End If
		  LabelHw5Value.TextColor = &c000000
		  
		  // Status section - GPS coordinates and altitude
		  LabelStat1Type.Text = "Altitude:"
		  Var theAlt As Double = inRocket.Lookup("alt", 0.0).DoubleValue
		  LabelStat1Value.Text = Format(theAlt, "0.0") + " m"
		  
		  LabelStat2Type.Text = "Latitude:"
		  Var theLat As Double = inRocket.Lookup("lat", 0.0).DoubleValue
		  LabelStat2Value.Text = Format(theLat, "0.000000")
		  
		  LabelStat3Type.Text = "Longitude:"
		  Var theLon As Double = inRocket.Lookup("lon", 0.0).DoubleValue
		  LabelStat3Value.Text = Format(theLon, "0.000000")
		  
		  LabelStatusMsg.Text = ""
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pIsRefreshing As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pRocketData() As Dictionary
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pSelectedRocketId As Integer = -1
	#tag EndProperty


#tag EndWindowCode

#tag Events ListRockets
	#tag Event
		Sub Change()
		  // Ignore change events during refresh to preserve selection
		  If pIsRefreshing Then Return
		  
		  // Selection changed - request info for selected rocket
		  ButtonRename.Enabled = Me.SelectedRowIndex >= 0
		  
		  If Me.SelectedRowIndex >= 0 Then
		    Var theRocketId As Integer = Me.RowTagAt(Me.SelectedRowIndex).IntegerValue
		    RequestRocketInfo(theRocketId)
		  Else
		    pSelectedRocketId = -1
		    ClearDetails
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonRename
	#tag Event
		Sub Action()
		  // Rename selected rocket using dialog
		  If ListRockets.SelectedRowIndex < 0 Then Return
		  
		  Var theRocketId As Integer = ListRockets.RowTagAt(ListRockets.SelectedRowIndex).IntegerValue
		  Var theCurrentName As String = GetRocketName(theRocketId)
		  If theCurrentName = "" Then
		    theCurrentName = "Rocket #" + Str(theRocketId)
		  End If
		  
		  // Show rename dialog
		  Var theDialog As New Dialog_RenameRocket
		  theDialog.ShowForRocket(theRocketId, theCurrentName)
		  
		  If Not theDialog.pWasCancelled And theDialog.pNewName <> "" Then
		    SetRocketName(theRocketId, theDialog.pNewName)
		    // Refresh list to show new name
		    HandleRocketList(pRocketData)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonRefreshList
	#tag Event
		Sub Action()
		  RefreshRocketList
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonClose
	#tag Event
		Sub Action()
		  Self.Hide
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events TimerRefresh
	#tag Event
		Sub Action()
		  // Auto-refresh rocket list
		  RefreshRocketList
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
