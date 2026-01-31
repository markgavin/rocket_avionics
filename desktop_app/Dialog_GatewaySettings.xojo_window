#tag Window
Begin Window Dialog_GatewaySettings
   Backdrop        =   0
   BackgroundColor =   &cFFFFFF00
   Composite       =   False
   DefaultLocation =   2
   Frame           =   1
   FullScreen      =   False
   HasBackgroundColor=   False
   HasCloseButton  =   True
   HasFullScreenButton=   False
   HasMaximizeButton=   False
   HasMinimizeButton=   False
   Height          =   280
   ImplicitInstance=   True
   MacProcID       =   0
   MaximumHeight   =   32000
   MaximumWidth    =   32000
   MenuBar         =   0
   MenuBarVisible  =   True
   MinimumHeight   =   280
   MinimumWidth    =   400
   Resizeable      =   False
   Title           =   "Gateway Settings"
   Type            =   1
   Visible         =   True
   Width           =   400
   Begin Label LabelTitle
      AllowAutoDeactivate=   True
      Bold            =   True
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   14.0
      FontUnit        =   0
      Height          =   25
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Multiline       =   False
      Scope           =   0
      Selectable      =   False
      TabIndex        =   0
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "Gateway Power Settings"
      TextAlignment   =   0
      TextColor       =   &c00000000
      Tooltip         =   ""
      Top             =   15
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   360
   End
   Begin Label LabelBacklight
      AllowAutoDeactivate=   True
      Bold            =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
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
      Text            =   "Display Backlight:"
      TextAlignment   =   2
      TextColor       =   &c00000000
      Tooltip         =   ""
      Top             =   55
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   120
   End
   Begin Slider SliderBacklight
      AllowAutoDeactivate=   True
      AllowFocus      =   True
      AllowLiveScrolling=   True
      AllowTabs       =   False
      Enabled         =   True
      Height          =   23
      Index           =   -2147483648
      InitialParent   =   ""
      Left            =   150
      LineStep        =   1
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Maximum         =   255
      Minimum         =   0
      PageStep        =   20
      Scope           =   0
      TabIndex        =   2
      TabPanelIndex   =   0
      TabStop         =   True
      TickStyle       =   0
      Tooltip         =   "Adjust display brightness (lower = cooler)"
      Top             =   52
      Transparent     =   False
      Value           =   64
      Visible         =   True
      Width           =   180
   End
   Begin Label LabelBacklightValue
      AllowAutoDeactivate=   True
      Bold            =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   340
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   True
      Multiline       =   False
      Scope           =   0
      Selectable      =   False
      TabIndex        =   3
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "64"
      TextAlignment   =   0
      TextColor       =   &c00000000
      Tooltip         =   ""
      Top             =   55
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   40
   End
   Begin Label LabelLoraTx
      AllowAutoDeactivate=   True
      Bold            =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
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
      Text            =   "LoRa TX Power:"
      TextAlignment   =   2
      TextColor       =   &c00000000
      Tooltip         =   ""
      Top             =   95
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   120
   End
   Begin Slider SliderLoraTx
      AllowAutoDeactivate=   True
      AllowFocus      =   True
      AllowLiveScrolling=   True
      AllowTabs       =   False
      Enabled         =   True
      Height          =   23
      Index           =   -2147483648
      InitialParent   =   ""
      Left            =   150
      LineStep        =   1
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Maximum         =   20
      Minimum         =   2
      PageStep        =   2
      Scope           =   0
      TabIndex        =   5
      TabPanelIndex   =   0
      TabStop         =   True
      TickStyle       =   0
      Tooltip         =   "LoRa radio transmit power (lower = cooler, shorter range)"
      Top             =   92
      Transparent     =   False
      Value           =   14
      Visible         =   True
      Width           =   180
   End
   Begin Label LabelLoraTxValue
      AllowAutoDeactivate=   True
      Bold            =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   340
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   True
      Multiline       =   False
      Scope           =   0
      Selectable      =   False
      TabIndex        =   6
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "14 dBm"
      TextAlignment   =   0
      TextColor       =   &c00000000
      Tooltip         =   ""
      Top             =   95
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   50
   End
   Begin Label LabelWifiTx
      AllowAutoDeactivate=   True
      Bold            =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   True
      Multiline       =   False
      Scope           =   0
      Selectable      =   False
      TabIndex        =   7
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "WiFi TX Power:"
      TextAlignment   =   2
      TextColor       =   &c00000000
      Tooltip         =   ""
      Top             =   135
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   120
   End
   Begin Slider SliderWifiTx
      AllowAutoDeactivate=   True
      AllowFocus      =   True
      AllowLiveScrolling=   True
      AllowTabs       =   False
      Enabled         =   True
      Height          =   23
      Index           =   -2147483648
      InitialParent   =   ""
      Left            =   150
      LineStep        =   1
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Maximum         =   20
      Minimum         =   0
      PageStep        =   2
      Scope           =   0
      TabIndex        =   8
      TabPanelIndex   =   0
      TabStop         =   True
      TickStyle       =   0
      Tooltip         =   "WiFi transmit power (lower = cooler, shorter range)"
      Top             =   132
      Transparent     =   False
      Value           =   8
      Visible         =   True
      Width           =   180
   End
   Begin Label LabelWifiTxValue
      AllowAutoDeactivate=   True
      Bold            =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   20
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   340
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   True
      Multiline       =   False
      Scope           =   0
      Selectable      =   False
      TabIndex        =   9
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "8 dBm"
      TextAlignment   =   0
      TextColor       =   &c00000000
      Tooltip         =   ""
      Top             =   135
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   50
   End
   Begin Label LabelNote
      AllowAutoDeactivate=   True
      Bold            =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   11.0
      FontUnit        =   0
      Height          =   40
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   True
      Left            =   20
      LockBottom      =   False
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Multiline       =   True
      Scope           =   0
      Selectable      =   False
      TabIndex        =   10
      TabPanelIndex   =   0
      TabStop         =   True
      Text            =   "Lower power settings reduce heat but may decrease range. Changes are applied immediately but not saved across restarts."
      TextAlignment   =   0
      TextColor       =   &c666666
      Tooltip         =   ""
      Top             =   175
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   360
   End
   Begin PushButton ButtonApply
      AllowAutoDeactivate=   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Apply"
      Default         =   True
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   22
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   195
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   0
      TabIndex        =   11
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   238
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   90
   End
   Begin PushButton ButtonClose
      AllowAutoDeactivate=   True
      Bold            =   False
      Cancel          =   True
      Caption         =   "Close"
      Default         =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   22
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   295
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   False
      LockRight       =   True
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   0
      TabIndex        =   12
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   238
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   90
   End
   Begin PushButton ButtonRefresh
      AllowAutoDeactivate=   True
      Bold            =   False
      Cancel          =   False
      Caption         =   "Refresh"
      Default         =   False
      Enabled         =   True
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   22
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   False
      LockTop         =   False
      MacButtonStyle  =   0
      Scope           =   0
      TabIndex        =   13
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   "Refresh settings from gateway"
      Top             =   238
      Transparent     =   False
      Underline       =   False
      Visible         =   True
      Width           =   90
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Opening()
		  // Request current settings when window opens
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendGetGatewaySettings
		  End If
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h0
		Sub HandleGatewaySettings(inBacklight As Integer, inBacklightMax As Integer, inLoraTxPower As Integer, inLoraTxMin As Integer, inLoraTxMax As Integer, inWifiTxPower As Integer, inWifiTxMin As Integer, inWifiTxMax As Integer)
		  // Update sliders with current values
		  SliderBacklight.Maximum = inBacklightMax
		  SliderBacklight.Value = inBacklight
		  LabelBacklightValue.Text = Str(inBacklight)

		  SliderLoraTx.Minimum = inLoraTxMin
		  SliderLoraTx.Maximum = inLoraTxMax
		  SliderLoraTx.Value = inLoraTxPower
		  LabelLoraTxValue.Text = Str(inLoraTxPower) + " dBm"

		  SliderWifiTx.Minimum = inWifiTxMin
		  SliderWifiTx.Maximum = inWifiTxMax
		  SliderWifiTx.Value = inWifiTxPower
		  LabelWifiTxValue.Text = Str(inWifiTxPower) + " dBm"
		End Sub
	#tag EndMethod


#tag EndWindowCode

#tag Events SliderBacklight
	#tag Event
		Sub ValueChanged()
		  LabelBacklightValue.Text = Str(Me.Value)
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events SliderLoraTx
	#tag Event
		Sub ValueChanged()
		  LabelLoraTxValue.Text = Str(Me.Value) + " dBm"
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events SliderWifiTx
	#tag Event
		Sub ValueChanged()
		  LabelWifiTxValue.Text = Str(Me.Value) + " dBm"
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonApply
	#tag Event
		Sub Pressed()
		  // Apply settings to gateway
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendGatewaySettings(SliderBacklight.Value, SliderLoraTx.Value, SliderWifiTx.Value)
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonClose
	#tag Event
		Sub Pressed()
		  Self.Close
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonRefresh
	#tag Event
		Sub Pressed()
		  // Request current settings
		  If Window_Main.pConnection <> Nil And Window_Main.pConnection.IsConnected Then
		    Window_Main.pConnection.SendGetGatewaySettings
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
