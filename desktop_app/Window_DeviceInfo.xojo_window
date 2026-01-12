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
   Height          =   440
   ImplicitInstance=   True
   LiveResize      =   "False"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   False
   MaxWidth        =   32000
   MenuBar         =   0
   MenuBarVisible  =   True
   MinHeight       =   64
   MinimizeButton  =   True
   MinWidth        =   64
   Placement       =   0
   Resizeable      =   False
   Title           =   "Device Information"
   Visible         =   False
   Width           =   450
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
         Top             =   38
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
         Top             =   38
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
         Top             =   60
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
         Top             =   60
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
         Top             =   82
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
         Top             =   82
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
      TabIndex        =   1
      TabPanelIndex   =   0
      TabStop         =   True
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   124
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
         Top             =   148
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
         Top             =   148
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
         Top             =   170
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
         Top             =   170
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
         Top             =   192
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
         Top             =   192
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
         Text            =   "SD Card:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   214
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
         Top             =   214
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
         Text            =   "RTC:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   236
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
         Top             =   236
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
   End
   Begin GroupBox GroupStatus
      AutoDeactivate  =   True
      Bold            =   False
      Caption         =   "Status"
      Enabled         =   True
      Height          =   122
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
      Top             =   278
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
         Top             =   302
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
         Top             =   302
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
         Text            =   "Samples:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   324
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
         Top             =   324
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
         Text            =   "SD Free:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   346
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
         Top             =   346
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   260
      End
      Begin Label LabelStat4Type
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
         TabIndex        =   6
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "Flights:"
         TextAlign       =   2
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   368
         Transparent     =   True
         Underline       =   False
         Visible         =   True
         Width           =   100
      End
      Begin Label LabelStat4Value
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
         TabIndex        =   7
         TabPanelIndex   =   0
         TabStop         =   False
         Text            =   "--"
         TextAlign       =   0
         TextColor       =   &c00000000
         TextFont        =   "System"
         TextSize        =   0.0
         TextUnit        =   0
         Top             =   368
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
      TabIndex        =   3
      TabPanelIndex   =   0
      TabStop         =   True
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   408
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
      TabIndex        =   4
      TabPanelIndex   =   0
      TabStop         =   False
      Text            =   ""
      TextAlign       =   0
      TextColor       =   &c66666600
      TextFont        =   "System"
      TextSize        =   0.0
      TextUnit        =   0
      Top             =   410
      Transparent     =   True
      Underline       =   False
      Visible         =   True
      Width           =   300
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Close()
		  // Save window position
		  Module_WindowSettings.SaveWindowPosition(Self, "Window_DeviceInfo")
		End Sub
	#tag EndEvent

	#tag Event
		Sub Open()
		  // Restore window position
		  Module_WindowSettings.LoadWindowPosition(Self, "Window_DeviceInfo")
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h0
		Sub ShowGatewayInfo(inInfo As Dictionary)
		  // Display gateway device information
		  Self.Title = "Gateway Device Information"

		  // Firmware
		  LabelDeviceValue.Text = "Ground Gateway"
		  LabelVersionValue.Text = "v" + inInfo.Lookup("version", "--").StringValue
		  LabelBuildValue.Text = inInfo.Lookup("build", "--").StringValue

		  // Hardware - Gateway specific
		  LabelHw1Type.Text = "LoRa Radio:"
		  LabelHw1Value.Text = If(inInfo.Lookup("lora", False).BooleanValue, "OK", "FAIL")
		  LabelHw1Value.TextColor = If(inInfo.Lookup("lora", False).BooleanValue, &c006600, &cCC0000)

		  LabelHw2Type.Text = "Ground Baro:"
		  LabelHw2Value.Text = If(inInfo.Lookup("bmp390", False).BooleanValue, "BMP390 OK", "Not Present")
		  LabelHw2Value.TextColor = If(inInfo.Lookup("bmp390", False).BooleanValue, &c006600, &c666666)

		  LabelHw3Type.Text = "TFT Display:"
		  LabelHw3Value.Text = If(inInfo.Lookup("display", False).BooleanValue, "OK", "FAIL")
		  LabelHw3Value.TextColor = If(inInfo.Lookup("display", False).BooleanValue, &c006600, &cCC0000)

		  // Hide unused hardware rows for gateway
		  LabelHw4Type.Visible = False
		  LabelHw4Value.Visible = False
		  LabelHw5Type.Visible = False
		  LabelHw5Value.Visible = False

		  // Resize hardware group
		  GroupHardware.Height = 100
		  GroupStatus.Top = 234

		  // Status - Gateway specific
		  GroupStatus.Caption = "Link Status"

		  LabelStat1Type.Text = "Flight Link:"
		  LabelStat1Value.Text = If(inInfo.Lookup("connected", False).BooleanValue, "CONNECTED", "Not Connected")
		  LabelStat1Value.TextColor = If(inInfo.Lookup("connected", False).BooleanValue, &c006600, &c666666)

		  LabelStat2Type.Text = "Packets RX:"
		  LabelStat2Value.Text = Str(inInfo.Lookup("rx", 0).IntegerValue)
		  LabelStat2Value.TextColor = &c000000

		  LabelStat3Type.Text = "Packets TX:"
		  LabelStat3Value.Text = Str(inInfo.Lookup("tx", 0).IntegerValue)
		  LabelStat3Value.TextColor = &c000000

		  LabelStat4Type.Text = "Signal:"
		  Var theRssi As Integer = inInfo.Lookup("rssi", 0).IntegerValue
		  Var theSnr As Integer = inInfo.Lookup("snr", 0).IntegerValue
		  LabelStat4Value.Text = Str(theRssi) + " dBm / " + Str(theSnr) + " dB SNR"
		  LabelStat4Value.TextColor = &c000000

		  // Ground reference info in status message
		  Var theGroundPres As Double = inInfo.Lookup("ground_pres", 0.0).DoubleValue
		  If theGroundPres > 0.0 Then
		    Var theGroundTemp As Double = inInfo.Lookup("ground_temp", 0.0).DoubleValue
		    LabelStatusMsg.Text = "Ground: " + Format(theGroundPres, "0") + " Pa, " + Format(theGroundTemp, "0.1") + " C"
		  Else
		    LabelStatusMsg.Text = ""
		  End If

		  // Adjust window height for gateway
		  Self.Height = 396

		  Self.Show
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub ShowFlightComputerInfo(inInfo As Dictionary)
		  // Display flight computer device information
		  Self.Title = "Flight Computer Device Information"

		  // Firmware
		  LabelDeviceValue.Text = "Flight Computer"
		  LabelVersionValue.Text = "v" + inInfo.Lookup("version", "--").StringValue
		  LabelBuildValue.Text = inInfo.Lookup("build", "--").StringValue

		  // Hardware - Flight computer specific
		  LabelHw1Type.Text = "LoRa Radio:"
		  LabelHw1Value.Text = If(inInfo.Lookup("lora", False).BooleanValue, "OK", "FAIL")
		  LabelHw1Value.TextColor = If(inInfo.Lookup("lora", False).BooleanValue, &c006600, &cCC0000)

		  LabelHw2Type.Text = "Barometer:"
		  LabelHw2Value.Text = If(inInfo.Lookup("bmp390", False).BooleanValue, "BMP390 OK", "FAIL")
		  LabelHw2Value.TextColor = If(inInfo.Lookup("bmp390", False).BooleanValue, &c006600, &cCC0000)

		  LabelHw3Type.Text = "OLED Display:"
		  LabelHw3Value.Text = If(inInfo.Lookup("oled", False).BooleanValue, "OK", "FAIL")
		  LabelHw3Value.TextColor = If(inInfo.Lookup("oled", False).BooleanValue, &c006600, &cCC0000)

		  LabelHw4Type.Text = "SD Card:"
		  LabelHw4Value.Text = If(inInfo.Lookup("sd", False).BooleanValue, "OK", "Not Present")
		  LabelHw4Value.TextColor = If(inInfo.Lookup("sd", False).BooleanValue, &c006600, &c666666)
		  LabelHw4Type.Visible = True
		  LabelHw4Value.Visible = True

		  LabelHw5Type.Text = "RTC:"
		  LabelHw5Value.Text = If(inInfo.Lookup("rtc", False).BooleanValue, "OK", "FAIL")
		  LabelHw5Value.TextColor = If(inInfo.Lookup("rtc", False).BooleanValue, &c006600, &cCC0000)
		  LabelHw5Type.Visible = True
		  LabelHw5Value.Visible = True

		  // Reset hardware group size
		  GroupHardware.Height = 144
		  GroupStatus.Top = 278

		  // Status - Flight computer specific
		  GroupStatus.Caption = "Flight Status"

		  LabelStat1Type.Text = "State:"
		  LabelStat1Value.Text = inInfo.Lookup("state", "--").StringValue.Uppercase
		  LabelStat1Value.TextColor = &c000000

		  LabelStat2Type.Text = "Samples:"
		  LabelStat2Value.Text = Str(inInfo.Lookup("samples", 0).IntegerValue)
		  LabelStat2Value.TextColor = &c000000

		  Var theSdFreeKb As Integer = inInfo.Lookup("sd_free_kb", 0).IntegerValue
		  LabelStat3Type.Text = "SD Free:"
		  If theSdFreeKb > 0 Then
		    LabelStat3Value.Text = Format(theSdFreeKb / 1024.0, "0.0") + " MB"
		  Else
		    LabelStat3Value.Text = "N/A"
		  End If
		  LabelStat3Value.TextColor = &c000000

		  LabelStat4Type.Text = "Flights:"
		  Var theFlightCount As Integer = inInfo.Lookup("flight_count", 0).IntegerValue
		  LabelStat4Value.Text = Str(theFlightCount)
		  LabelStat4Value.TextColor = &c000000

		  // GPS status in status message
		  Var theHasGps As Boolean = inInfo.Lookup("gps", False).BooleanValue
		  If theHasGps Then
		    LabelStatusMsg.Text = "GPS: OK"
		  Else
		    LabelStatusMsg.Text = "GPS: Not Present"
		  End If

		  // Reset window height for flight computer
		  Self.Height = 440

		  Self.Show
		End Sub
	#tag EndMethod


#tag EndWindowCode

#tag Events ButtonClose
	#tag Event
		Sub Action()
		  Self.Hide
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
