#tag Window
Begin Window Window_DeviceStorage
   BackColor       =   &cFFFFFF00
   Backdrop        =   0
   CloseButton     =   True
   Composite       =   False
   Frame           =   0
   FullScreen      =   False
   HasBackColor    =   False
   HasFullScreenButton=   False
   Height          =   500
   ImplicitInstance=   True
   LiveResize      =   "True"
   MacProcID       =   0
   MaxHeight       =   32000
   MaximizeButton  =   True
   MaxWidth        =   32000
   MenuBar         =   0
   MenuBarVisible  =   True
   MinHeight       =   400
   MinimizeButton  =   True
   MinWidth        =   600
   Placement       =   0
   Resizeable      =   True
   Title           =   "Flight Computer Storage"
   Visible         =   False
   Width           =   700
   Begin TabPanel TabPanelStorage
      AutoDeactivate  =   True
      Bold            =   False
      Enabled         =   True
      Height          =   440
      Index           =   -2147483648
      Left            =   20
      LockBottom      =   True
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Panels          =   "Flash Memory\rSD Card"
      Scope           =   2
      TabIndex        =   0
      Top             =   20
      Value           =   0
      Visible         =   True
      Width           =   660
      Begin Listbox ListFlash
         AllowRowDragging=   False
         AllowRowReordering=   False
         AutoDeactivate  =   True
         AutoHideScrollbars=   True
         Bold            =   False
         Border          =   True
         ColumnCount     =   5
         ColumnsResizable=   True
         ColumnWidths    =   "60,150,100,100,100"
         DefaultRowHeight=   22
         Enabled         =   True
         GridLinesHorizontal=   0
         GridLinesVertical=   0
         HasHeading      =   True
         HeadingIndex    =   -1
         Height          =   320
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         InitialValue    =   "Slot	Date/Time	Max Alt (m)	Duration (s)	Size"
         Left            =   30
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         RequiresSelection=   False
         Scope           =   2
         ScrollbarHorizontal=   False
         ScrollbarVertical=   True
         SelectionType   =   0
         TabIndex        =   0
         Top             =   60
         Visible         =   True
         Width           =   640
      End
      Begin PushButton ButtonFlashRefresh
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Refresh"
         Default         =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   30
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         Scope           =   2
         TabIndex        =   1
         Top             =   395
         Visible         =   True
         Width           =   80
      End
      Begin PushButton ButtonFlashDownload
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Download"
         Default         =   False
         Enabled         =   False
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   120
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         Scope           =   2
         TabIndex        =   2
         Top             =   395
         Visible         =   True
         Width           =   80
      End
      Begin PushButton ButtonFlashDelete
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Delete"
         Default         =   False
         Enabled         =   False
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   210
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         Scope           =   2
         TabIndex        =   3
         Top             =   395
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelFlashStatus
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   310
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   False
         Scope           =   2
         TabIndex        =   4
         Text            =   "Not connected"
         TextAlign       =   0
         TextColor       =   &c80808000
         Top             =   395
         Visible         =   True
         Width           =   360
      End
      Begin Listbox ListSD
         AllowRowDragging=   False
         AllowRowReordering=   False
         AutoDeactivate  =   True
         AutoHideScrollbars=   True
         Bold            =   False
         Border          =   True
         ColumnCount     =   4
         ColumnsResizable=   True
         ColumnWidths    =   "250,100,120,100"
         DefaultRowHeight=   22
         Enabled         =   True
         GridLinesHorizontal=   0
         GridLinesVertical=   0
         HasHeading      =   True
         HeadingIndex    =   -1
         Height          =   320
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         InitialValue    =   "Filename	Size	Date	Type"
         Left            =   30
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         RequiresSelection=   False
         Scope           =   2
         ScrollbarHorizontal=   False
         ScrollbarVertical=   True
         SelectionType   =   0
         TabIndex        =   5
         Top             =   60
         Visible         =   False
         Width           =   640
      End
      Begin PushButton ButtonSDRefresh
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Refresh"
         Default         =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   30
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         Scope           =   2
         TabIndex        =   6
         Top             =   395
         Visible         =   False
         Width           =   80
      End
      Begin PushButton ButtonSDDownload
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Download"
         Default         =   False
         Enabled         =   False
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   120
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         Scope           =   2
         TabIndex        =   7
         Top             =   395
         Visible         =   False
         Width           =   80
      End
      Begin PushButton ButtonSDDelete
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Delete"
         Default         =   False
         Enabled         =   False
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   210
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         Scope           =   2
         TabIndex        =   8
         Top             =   395
         Visible         =   False
         Width           =   80
      End
      Begin Label LabelSDStatus
         AutoDeactivate  =   True
         Bold            =   False
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   310
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   False
         Scope           =   2
         TabIndex        =   9
         Text            =   "Not connected"
         TextAlign       =   0
         TextColor       =   &c80808000
         Top             =   395
         Visible         =   False
         Width           =   360
      End
      Begin ProgressBar ProgressDownload
         AutoDeactivate  =   True
         Enabled         =   True
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   30
         LockBottom      =   True
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   False
         Maximum         =   100
         Scope           =   2
         TabIndex        =   10
         Top             =   425
         Value           =   0
         Visible         =   False
         Width           =   640
      End
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Open()
		  // Restore window position
		  Module_WindowSettings.LoadWindowPosition(Self, "Window_DeviceStorage")

		  // Get connection reference from main window
		  pConnection = Window_Main.pConnection

		  // Check if connected
		  UpdateConnectionStatus
		End Sub
	#tag EndEvent

	#tag Event
		Sub Close()
		  // Save window position
		  Module_WindowSettings.SaveWindowPosition(Self, "Window_DeviceStorage")
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h0
		Sub RefreshFlashList()
		  // Request flash memory contents from flight computer
		  If pConnection = Nil Or Not pConnection.IsConnected Then
		    LabelFlashStatus.Text = "Not connected"
		    Return
		  End If

		  LabelFlashStatus.Text = "Requesting flash list..."
		  ListFlash.RemoveAllRows

		  // Send flash list command
		  pConnection.SendCommand("flash_list")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub RefreshSDList()
		  // Request SD card contents from flight computer
		  If pConnection = Nil Or Not pConnection.IsConnected Then
		    LabelSDStatus.Text = "Not connected"
		    Return
		  End If

		  LabelSDStatus.Text = "Requesting SD card list..."
		  ListSD.RemoveAllRows

		  // Send SD list command
		  pConnection.SendCommand("sd_list")
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub UpdateConnectionStatus()
		  If pConnection <> Nil And pConnection.IsConnected Then
		    LabelFlashStatus.Text = "Connected - Click Refresh to load"
		    LabelSDStatus.Text = "Connected - Click Refresh to load"
		  Else
		    LabelFlashStatus.Text = "Not connected"
		    LabelSDStatus.Text = "Not connected"
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub HandleFlashListResponse(inSlots() As Dictionary)
		  // Populate flash list from response
		  ListFlash.RemoveAllRows

		  For Each theSlot As Dictionary In inSlots
		    ListFlash.AddRow( _
		      Str(theSlot.Lookup("slot", 0)), _
		      theSlot.Lookup("timestamp", "").StringValue, _
		      Format(theSlot.Lookup("max_alt", 0.0).DoubleValue, "0.0"), _
		      Format(theSlot.Lookup("duration", 0.0).DoubleValue / 1000.0, "0.0"), _
		      FormatBytes(theSlot.Lookup("size", 0).IntegerValue))
		  Next

		  LabelFlashStatus.Text = Str(inSlots.Count) + " flight(s) stored"
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub HandleSDListResponse(inFiles() As Dictionary)
		  // Populate SD list from response
		  ListSD.RemoveAllRows

		  For Each theFile As Dictionary In inFiles
		    Var theType As String = "File"
		    If theFile.Lookup("is_dir", False).BooleanValue Then
		      theType = "Folder"
		    ElseIf theFile.Lookup("name", "").StringValue.EndsWith(".csv") Then
		      theType = "Flight Log"
		    End If

		    ListSD.AddRow( _
		      theFile.Lookup("name", "").StringValue, _
		      FormatBytes(theFile.Lookup("size", 0).IntegerValue), _
		      theFile.Lookup("date", "").StringValue, _
		      theType)
		  Next

		  LabelSDStatus.Text = Str(inFiles.Count) + " file(s)"
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function FormatBytes(inBytes As Integer) As String
		  If inBytes < 1024 Then
		    Return Str(inBytes) + " B"
		  ElseIf inBytes < 1024 * 1024 Then
		    Return Format(inBytes / 1024.0, "0.0") + " KB"
		  Else
		    Return Format(inBytes / (1024.0 * 1024.0), "0.0") + " MB"
		  End If
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub ShowDownloadProgress(inPercent As Integer)
		  ProgressDownload.Visible = True
		  ProgressDownload.Value = inPercent
		  If inPercent >= 100 Then
		    ProgressDownload.Visible = False
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub HandleStorageList(inIsSd As Boolean, inFiles() As Dictionary)
		  // Handle storage list response from flight computer
		  If inIsSd Then
		    HandleSDListResponse(inFiles)
		  Else
		    HandleFlashListResponse(inFiles)
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub HandleStorageData(inIsSd As Boolean, inOffset As Integer, inTotal As Integer, inHexData As String)
		  // Handle storage data chunk from flight computer

		  // Convert hex string to binary data
		  Var theBinaryData As String = HexToBytes(inHexData)

		  // Store total size on first chunk
		  If inOffset = 0 Then
		    pDownloadTotal = inTotal
		    pDownloadData = ""
		  End If

		  // Accumulate data
		  pDownloadData = pDownloadData + theBinaryData
		  pDownloadOffset = inOffset + theBinaryData.Bytes

		  // Update progress
		  If pDownloadTotal > 0 Then
		    Var thePercent As Integer = (pDownloadOffset * 100) / pDownloadTotal
		    ShowDownloadProgress(thePercent)

		    If inIsSd Then
		      LabelSDStatus.Text = "Downloading: " + Str(thePercent) + "%"
		    Else
		      LabelFlashStatus.Text = "Downloading: " + Str(thePercent) + "%"
		    End If
		  End If

		  // Check if download complete
		  If pDownloadOffset >= pDownloadTotal Then
		    CompleteDownload(inIsSd)
		  Else
		    // Request next chunk
		    If pConnection <> Nil And pConnection.IsConnected Then
		      pConnection.SendStorageRead(inIsSd, pDownloadFilename, pDownloadOffset)
		    End If
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub CompleteDownload(inIsSd As Boolean)
		  // Download complete - save the file
		  ShowDownloadProgress(100)

		  // Prompt user for save location
		  Var theDlg As New SaveFileDialog
		  theDlg.SuggestedFileName = pDownloadFilename
		  theDlg.Title = "Save Downloaded File"

		  If pDownloadFilename.EndsWith(".csv") Then
		    Var theFilter As New FileType
		    theFilter.Name = "CSV Files"
		    theFilter.Extensions = "csv"
		    theDlg.Filter = theFilter
		  End If

		  Var theFile As FolderItem = theDlg.ShowModal

		  If theFile <> Nil Then
		    Try
		      Var theStream As BinaryStream = BinaryStream.Create(theFile, True)
		      theStream.Write(pDownloadData)
		      theStream.Close

		      If inIsSd Then
		        LabelSDStatus.Text = "Saved: " + theFile.Name
		      Else
		        LabelFlashStatus.Text = "Saved: " + theFile.Name
		      End If

		    Catch e As IOException
		      MsgBox("Error saving file: " + e.Message)
		    End Try
		  Else
		    If inIsSd Then
		      LabelSDStatus.Text = "Download cancelled"
		    Else
		      LabelFlashStatus.Text = "Download cancelled"
		    End If
		  End If

		  // Reset download state
		  pDownloadFilename = ""
		  pDownloadOffset = 0
		  pDownloadTotal = 0
		  pDownloadData = ""
		  ProgressDownload.Visible = False
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function HexToBytes(inHex As String) As String
		  // Convert hex string to binary data
		  Var theResult As String = ""
		  Var i As Integer = 0

		  While i < inHex.Length - 1
		    Var theHexByte As String = inHex.Middle(i, 2)
		    Var theValue As Integer = Val("&h" + theHexByte)
		    theResult = theResult + Chr(theValue)
		    i = i + 2
		  Wend

		  Return theResult
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub StartDownload(inIsSd As Boolean, inFilename As String)
		  // Start downloading a file
		  pDownloadFilename = inFilename
		  pDownloadOffset = 0
		  pDownloadTotal = 0
		  pDownloadData = ""
		  pDownloadIsSd = inIsSd

		  ShowDownloadProgress(0)

		  If pConnection <> Nil And pConnection.IsConnected Then
		    pConnection.SendStorageRead(inIsSd, inFilename, 0)
		  End If
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pConnection As FlightConnection
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadFilename As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadOffset As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadTotal As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadData As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadIsSd As Boolean = True
	#tag EndProperty


#tag EndWindowCode

#tag Events TabPanelStorage
	#tag Event
		Sub Change()
		  // Show/hide controls based on selected tab
		  Var theShowFlash As Boolean = (Me.Value = 0)
		  Var theShowSD As Boolean = (Me.Value = 1)

		  ListFlash.Visible = theShowFlash
		  ButtonFlashRefresh.Visible = theShowFlash
		  ButtonFlashDownload.Visible = theShowFlash
		  ButtonFlashDelete.Visible = theShowFlash
		  LabelFlashStatus.Visible = theShowFlash

		  ListSD.Visible = theShowSD
		  ButtonSDRefresh.Visible = theShowSD
		  ButtonSDDownload.Visible = theShowSD
		  ButtonSDDelete.Visible = theShowSD
		  LabelSDStatus.Visible = theShowSD
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ListFlash
	#tag Event
		Sub Change()
		  // Enable/disable buttons based on selection
		  Var theHasSelection As Boolean = (Me.SelectedRowIndex >= 0)
		  ButtonFlashDownload.Enabled = theHasSelection
		  ButtonFlashDelete.Enabled = theHasSelection
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonFlashRefresh
	#tag Event
		Sub Action()
		  RefreshFlashList
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonFlashDownload
	#tag Event
		Sub Action()
		  If ListFlash.SelectedRowIndex < 0 Then Return

		  // For flash, slot is stored as "slot_X" filename
		  Var theSlot As String = ListFlash.Cell(ListFlash.SelectedRowIndex, 0)
		  Var theFilename As String = "slot_" + theSlot + ".bin"
		  LabelFlashStatus.Text = "Downloading slot " + theSlot + "..."

		  // Start download
		  StartDownload(False, theFilename)
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonFlashDelete
	#tag Event
		Sub Action()
		  If ListFlash.SelectedRowIndex < 0 Then Return

		  Var theSlot As String = ListFlash.Cell(ListFlash.SelectedRowIndex, 0)
		  Var theFilename As String = "slot_" + theSlot + ".bin"

		  // Confirm deletion
		  Var theResult As Integer = MsgBox("Delete flight in slot " + theSlot + "?" + EndOfLine + _
		    "This cannot be undone.", 1, "Confirm Delete")

		  If theResult = 1 Then
		    LabelFlashStatus.Text = "Deleting slot " + theSlot + "..."
		    If pConnection <> Nil And pConnection.IsConnected Then
		      pConnection.SendStorageDelete(False, theFilename)
		    End If
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ListSD
	#tag Event
		Sub Change()
		  // Enable/disable buttons based on selection
		  Var theHasSelection As Boolean = (Me.SelectedRowIndex >= 0)
		  ButtonSDDownload.Enabled = theHasSelection
		  ButtonSDDelete.Enabled = theHasSelection
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonSDRefresh
	#tag Event
		Sub Action()
		  RefreshSDList
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonSDDownload
	#tag Event
		Sub Action()
		  If ListSD.SelectedRowIndex < 0 Then Return

		  Var theFilename As String = ListSD.Cell(ListSD.SelectedRowIndex, 0)
		  LabelSDStatus.Text = "Downloading " + theFilename + "..."

		  // Start download
		  StartDownload(True, theFilename)
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonSDDelete
	#tag Event
		Sub Action()
		  If ListSD.SelectedRowIndex < 0 Then Return

		  Var theFilename As String = ListSD.Cell(ListSD.SelectedRowIndex, 0)

		  // Confirm deletion
		  Var theResult As Integer = MsgBox("Delete " + theFilename + "?" + EndOfLine + _
		    "This cannot be undone.", 1, "Confirm Delete")

		  If theResult = 1 Then
		    LabelSDStatus.Text = "Deleting " + theFilename + "..."
		    If pConnection <> Nil And pConnection.IsConnected Then
		      pConnection.SendStorageDelete(True, theFilename)
		    End If
		  End If
		End Sub
	#tag EndEvent
#tag EndEvents
