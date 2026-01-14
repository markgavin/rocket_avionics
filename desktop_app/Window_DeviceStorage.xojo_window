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
   MenuBar         =   4294967314
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
      FontName        =   "System"
      FontSize        =   0.0
      FontUnit        =   0
      Height          =   440
      Index           =   -2147483648
      InitialParent   =   ""
      Italic          =   False
      Left            =   20
      LockBottom      =   True
      LockedInPosition=   False
      LockLeft        =   True
      LockRight       =   True
      LockTop         =   True
      Panels          =   "Flash Memory\rSD Card"
      Scope           =   2
      SmallTabs       =   False
      TabDefinition   =   "Tab 0\rTab 1"
      TabIndex        =   0
      TabPanelIndex   =   0
      TabStop         =   True
      Tooltip         =   ""
      Top             =   20
      Transparent     =   False
      Underline       =   False
      Value           =   0
      Visible         =   True
      Width           =   660
      Begin Listbox ListFlash
         AllowExpandableRows=   False
         AllowFocusRing  =   True
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
         DropIndicatorVisible=   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         GridLinesHorizontal=   0
         GridLinesVertical=   0
         HasHeading      =   True
         HeadingIndex    =   -1
         Height          =   320
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         InitialValue    =   "Slot	Date/Time	Max Alt (m)	Duration (s)	Size"
         Italic          =   False
         Left            =   30
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         RequiresSelection=   False
         Scope           =   2
         ScrollbarHorizontal=   False
         ScrollbarVertical=   True
         SelectionType   =   0
         TabIndex        =   0
         TabPanelIndex   =   1
         TabStop         =   True
         Tooltip         =   ""
         Top             =   60
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   640
         _ScrollOffset   =   0
         _ScrollWidth    =   -1
      End
      Begin PushButton ButtonFlashRefresh
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Refresh"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Italic          =   False
         Left            =   30
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   1
         TabPanelIndex   =   1
         TabStop         =   True
         Tooltip         =   ""
         Top             =   395
         Transparent     =   False
         Underline       =   False
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
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Italic          =   False
         Left            =   120
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   2
         TabPanelIndex   =   1
         TabStop         =   True
         Tooltip         =   ""
         Top             =   395
         Transparent     =   False
         Underline       =   False
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
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Italic          =   False
         Left            =   210
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   3
         TabPanelIndex   =   1
         TabStop         =   True
         Tooltip         =   ""
         Top             =   395
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   80
      End
      Begin Label LabelFlashStatus
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
         InitialParent   =   "TabPanelStorage"
         Italic          =   False
         Left            =   310
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   False
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   4
         TabPanelIndex   =   1
         TabStop         =   True
         Text            =   "Not connected"
         TextAlign       =   0
         TextColor       =   &c80808000
         Tooltip         =   ""
         Top             =   395
         Transparent     =   False
         Underline       =   False
         Visible         =   True
         Width           =   360
      End
      Begin Listbox ListSD
         AllowExpandableRows=   False
         AllowFocusRing  =   True
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
         DropIndicatorVisible=   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         GridLinesHorizontal=   0
         GridLinesVertical=   0
         HasHeading      =   True
         HeadingIndex    =   -1
         Height          =   320
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         InitialValue    =   "Filename	Size	Date	Type"
         Italic          =   False
         Left            =   30
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   True
         RequiresSelection=   False
         Scope           =   2
         ScrollbarHorizontal=   False
         ScrollbarVertical=   True
         SelectionType   =   0
         TabIndex        =   5
         TabPanelIndex   =   1
         TabStop         =   True
         Tooltip         =   ""
         Top             =   60
         Transparent     =   False
         Underline       =   False
         Visible         =   False
         Width           =   640
         _ScrollOffset   =   0
         _ScrollWidth    =   -1
      End
      Begin PushButton ButtonSDRefresh
         AutoDeactivate  =   True
         Bold            =   False
         Cancel          =   False
         Caption         =   "Refresh"
         Default         =   False
         Enabled         =   True
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Italic          =   False
         Left            =   30
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   6
         TabPanelIndex   =   1
         TabStop         =   True
         Tooltip         =   ""
         Top             =   395
         Transparent     =   False
         Underline       =   False
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
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Italic          =   False
         Left            =   120
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   7
         TabPanelIndex   =   1
         TabStop         =   True
         Tooltip         =   ""
         Top             =   395
         Transparent     =   False
         Underline       =   False
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
         FontName        =   "System"
         FontSize        =   0.0
         FontUnit        =   0
         Height          =   20
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Italic          =   False
         Left            =   210
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   False
         LockTop         =   False
         MacButtonStyle  =   0
         Scope           =   2
         TabIndex        =   8
         TabPanelIndex   =   1
         TabStop         =   True
         Tooltip         =   ""
         Top             =   395
         Transparent     =   False
         Underline       =   False
         Visible         =   False
         Width           =   80
      End
      Begin Label LabelSDStatus
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
         InitialParent   =   "TabPanelStorage"
         Italic          =   False
         Left            =   310
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   False
         Multiline       =   False
         Scope           =   2
         Selectable      =   False
         TabIndex        =   9
         TabPanelIndex   =   1
         TabStop         =   True
         Text            =   "Not connected"
         TextAlign       =   0
         TextColor       =   &c80808000
         Tooltip         =   ""
         Top             =   395
         Transparent     =   False
         Underline       =   False
         Visible         =   False
         Width           =   360
      End
      Begin ProgressBar ProgressDownload
         AutoDeactivate  =   True
         Enabled         =   True
         Height          =   20
         Indeterminate   =   False
         Index           =   -2147483648
         InitialParent   =   "TabPanelStorage"
         Left            =   30
         LockBottom      =   True
         LockedInPosition=   False
         LockLeft        =   True
         LockRight       =   True
         LockTop         =   False
         Maximum         =   100
         Scope           =   2
         TabIndex        =   10
         TabPanelIndex   =   1
         TabStop         =   True
         Tooltip         =   ""
         Top             =   425
         Transparent     =   False
         Value           =   0.0
         Visible         =   False
         Width           =   640
      End
   End
End
#tag EndWindow

#tag WindowCode
	#tag Event
		Sub Close()
		  // Save window position
		  Module_WindowSettings.SaveWindowPosition(Self, "Window_DeviceStorage")
		End Sub
	#tag EndEvent

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
		Sub HandleFlashListResponse(inCount As Integer, inFlights() As Dictionary)
		  // Populate flash list from response
		  ListFlash.RemoveAllRows

		  For Each theFlight As Dictionary In inFlights
		    Var theSlot As Integer = theFlight.Lookup("slot", 0)
		    Var theFlightId As Integer = theFlight.Lookup("id", 0)
		    Var theMaxAlt As Double = theFlight.Lookup("altitude", 0.0)
		    Var theTimeMs As Integer = theFlight.Lookup("time_ms", 0)
		    Var theSamples As Integer = theFlight.Lookup("samples", 0)

		    // Duration in seconds
		    Var theDuration As Double = theTimeMs / 1000.0

		    // Estimate size (52 bytes per sample + 80 byte header)
		    Var theSize As Integer = (theSamples * 52) + 80

		    ListFlash.AddRow( _
		    Str(theSlot), _
		    "Flight #" + Str(theFlightId), _
		    Format(theMaxAlt, "0.0"), _
		    Format(theDuration, "0.0"), _
		    FormatBytes(theSize))

		    // Store slot and sample count in row tag for later download
		    ListFlash.RowTagAt(ListFlash.LastAddedRowIndex) = theFlight
		  Next

		  LabelFlashStatus.Text = Str(inCount) + " flight(s) stored"
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

	#tag Method, Flags = &h0
		Sub HandleStorageList(inIsSd As Boolean, inFiles() As Dictionary)
		  // Handle storage list response from flight computer
		  If inIsSd Then
		    HandleSDListResponse(inFiles)
		  Else
		    HandleFlashListResponse(inFiles.Count, inFiles)
		  End If
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
		Sub RefreshFlashList()
		  // Request flash memory contents from flight computer
		  If pConnection = Nil Or Not pConnection.IsConnected Then
		    LabelFlashStatus.Text = "Not connected"
		    Return
		  End If

		  LabelFlashStatus.Text = "Requesting flash list..."
		  ListFlash.RemoveAllRows

		  // Send flash list command
		  pConnection.SendFlashList
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
		Sub StartDownload(inIsSd As Boolean, inFilename As String)
		  // Start downloading a file (SD card only)
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

	#tag Method, Flags = &h0
		Sub StartFlashDownload(inSlot As Integer, inTotalSamples As Integer)
		  // Start downloading a flight from flash
		  pFlashDownloadSlot = inSlot
		  pFlashDownloadSample = 0
		  pFlashDownloadTotal = inTotalSamples
		  pFlashDownloadData = ""
		  pDownloadIsSd = False

		  ShowDownloadProgress(0)

		  If pConnection <> Nil And pConnection.IsConnected Then
		    pConnection.SendFlashRead(inSlot, 0)
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub HandleFlashData(inSlot As Integer, inStart As Integer, inTotal As Integer, inCount As Integer, inHexData As String)
		  // Handle flash data chunk from flight computer

		  // Convert hex string to binary data
		  Var theBinaryData As String = HexToBytes(inHexData)

		  // Store total on first chunk
		  If inStart = 0 Then
		    pFlashDownloadTotal = inTotal
		    pFlashDownloadData = ""
		  End If

		  // Accumulate data
		  pFlashDownloadData = pFlashDownloadData + theBinaryData
		  pFlashDownloadSample = inStart + inCount

		  // Update progress
		  If pFlashDownloadTotal > 0 Then
		    Var thePercent As Integer = (pFlashDownloadSample * 100) / pFlashDownloadTotal
		    ShowDownloadProgress(thePercent)
		    LabelFlashStatus.Text = "Downloading: " + Str(thePercent) + "% (" + Str(pFlashDownloadSample) + "/" + Str(pFlashDownloadTotal) + " samples)"
		  End If

		  // Check if download complete
		  If pFlashDownloadSample >= pFlashDownloadTotal Then
		    CompleteFlashDownload
		  Else
		    // Request next chunk
		    If pConnection <> Nil And pConnection.IsConnected Then
		      pConnection.SendFlashRead(inSlot, pFlashDownloadSample)
		    End If
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub CompleteFlashDownload()
		  // Flash download complete - save as CSV
		  ShowDownloadProgress(100)

		  // Prompt user for save location
		  Var theDlg As New SaveFileDialog
		  theDlg.SuggestedFileName = "flight_slot" + Str(pFlashDownloadSlot) + ".csv"
		  theDlg.Title = "Save Flight Data"

		  Var theFilter As New FileType
		  theFilter.Name = "CSV Files"
		  theFilter.Extensions = "csv"
		  theDlg.Filter = theFilter

		  Var theFile As FolderItem = theDlg.ShowModal

		  If theFile <> Nil Then
		    Try
		      // Convert binary sample data to CSV
		      Var theCsv As String = ConvertFlashDataToCsv(pFlashDownloadData)

		      Var theStream As TextOutputStream = TextOutputStream.Create(theFile)
		      theStream.Write(theCsv)
		      theStream.Close

		      LabelFlashStatus.Text = "Saved: " + theFile.Name

		    Catch e As IOException
		      MsgBox("Error saving file: " + e.Message)
		    End Try
		  Else
		    LabelFlashStatus.Text = "Download cancelled"
		  End If

		  // Reset download state
		  pFlashDownloadSlot = -1
		  pFlashDownloadSample = 0
		  pFlashDownloadTotal = 0
		  pFlashDownloadData = ""
		  ProgressDownload.Visible = False
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function ConvertFlashDataToCsv(inData As String) As String
		  // Convert binary flash sample data to CSV format
		  // Each sample is 52 bytes (FlightSample structure)

		  Var theCsv As String = "Time (ms),Altitude (m),Velocity (m/s),Pressure (Pa),Temperature (C),"
		  theCsv = theCsv + "GPS Lat,GPS Lon,GPS Speed (m/s),GPS Heading,GPS Sats,"
		  theCsv = theCsv + "Accel X (g),Accel Y (g),Accel Z (g),"
		  theCsv = theCsv + "Gyro X (dps),Gyro Y (dps),Gyro Z (dps),"
		  theCsv = theCsv + "Mag X,Mag Y,Mag Z,State" + EndOfLine

		  Var theSampleSize As Integer = 52
		  Var theOffset As Integer = 0

		  While theOffset + theSampleSize <= inData.Bytes
		    Var theSample As String = inData.Middle(theOffset, theSampleSize)

		    // Parse sample structure (little-endian)
		    Var theTimeMs As UInt32 = ParseUInt32(theSample, 0)
		    Var theAltCm As Int32 = ParseInt32(theSample, 4)
		    Var theVelCmps As Int16 = ParseInt16(theSample, 8)
		    Var thePresPa As UInt32 = ParseUInt32(theSample, 10)
		    Var theTempC10 As Int16 = ParseInt16(theSample, 14)

		    Var theGpsLat As Int32 = ParseInt32(theSample, 16)
		    Var theGpsLon As Int32 = ParseInt32(theSample, 20)
		    Var theGpsSpeed As Int16 = ParseInt16(theSample, 24)
		    Var theGpsHdg As UInt16 = ParseUInt16(theSample, 26)
		    Var theGpsSats As Integer = theSample.Middle(28, 1).Asc

		    Var theAccelX As Int16 = ParseInt16(theSample, 29)
		    Var theAccelY As Int16 = ParseInt16(theSample, 31)
		    Var theAccelZ As Int16 = ParseInt16(theSample, 33)

		    Var theGyroX As Int16 = ParseInt16(theSample, 35)
		    Var theGyroY As Int16 = ParseInt16(theSample, 37)
		    Var theGyroZ As Int16 = ParseInt16(theSample, 39)

		    Var theMagX As Int16 = ParseInt16(theSample, 41)
		    Var theMagY As Int16 = ParseInt16(theSample, 43)
		    Var theMagZ As Int16 = ParseInt16(theSample, 45)

		    Var theState As Integer = theSample.Middle(47, 1).Asc

		    // Format row
		    theCsv = theCsv + Str(theTimeMs) + ","
		    theCsv = theCsv + Format(theAltCm / 100.0, "0.00") + ","
		    theCsv = theCsv + Format(theVelCmps / 100.0, "0.00") + ","
		    theCsv = theCsv + Str(thePresPa) + ","
		    theCsv = theCsv + Format(theTempC10 / 10.0, "0.0") + ","

		    theCsv = theCsv + Format(theGpsLat / 1000000.0, "0.000000") + ","
		    theCsv = theCsv + Format(theGpsLon / 1000000.0, "0.000000") + ","
		    theCsv = theCsv + Format(theGpsSpeed / 100.0, "0.00") + ","
		    theCsv = theCsv + Format(theGpsHdg / 10.0, "0.0") + ","
		    theCsv = theCsv + Str(theGpsSats) + ","

		    theCsv = theCsv + Format(theAccelX / 1000.0, "0.000") + ","
		    theCsv = theCsv + Format(theAccelY / 1000.0, "0.000") + ","
		    theCsv = theCsv + Format(theAccelZ / 1000.0, "0.000") + ","

		    theCsv = theCsv + Format(theGyroX / 10.0, "0.0") + ","
		    theCsv = theCsv + Format(theGyroY / 10.0, "0.0") + ","
		    theCsv = theCsv + Format(theGyroZ / 10.0, "0.0") + ","

		    theCsv = theCsv + Str(theMagX) + ","
		    theCsv = theCsv + Str(theMagY) + ","
		    theCsv = theCsv + Str(theMagZ) + ","
		    theCsv = theCsv + Str(theState) + EndOfLine

		    theOffset = theOffset + theSampleSize
		  Wend

		  Return theCsv
		End Function
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function ParseInt16(inData As String, inOffset As Integer) As Int16
		  Var theLow As Integer = inData.Middle(inOffset, 1).Asc
		  Var theHigh As Integer = inData.Middle(inOffset + 1, 1).Asc
		  Var theValue As Integer = theLow Or (theHigh * 256)
		  If theValue >= 32768 Then theValue = theValue - 65536
		  Return theValue
		End Function
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function ParseUInt16(inData As String, inOffset As Integer) As UInt16
		  Var theLow As Integer = inData.Middle(inOffset, 1).Asc
		  Var theHigh As Integer = inData.Middle(inOffset + 1, 1).Asc
		  Return theLow Or (theHigh * 256)
		End Function
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function ParseInt32(inData As String, inOffset As Integer) As Int32
		  Var theByte0 As Integer = inData.Middle(inOffset, 1).Asc
		  Var theByte1 As Integer = inData.Middle(inOffset + 1, 1).Asc
		  Var theByte2 As Integer = inData.Middle(inOffset + 2, 1).Asc
		  Var theByte3 As Integer = inData.Middle(inOffset + 3, 1).Asc
		  Return theByte0 Or (theByte1 * 256) Or (theByte2 * 65536) Or (theByte3 * 16777216)
		End Function
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function ParseUInt32(inData As String, inOffset As Integer) As UInt32
		  Var theByte0 As Integer = inData.Middle(inOffset, 1).Asc
		  Var theByte1 As Integer = inData.Middle(inOffset + 1, 1).Asc
		  Var theByte2 As Integer = inData.Middle(inOffset + 2, 1).Asc
		  Var theByte3 As Integer = inData.Middle(inOffset + 3, 1).Asc
		  Return theByte0 Or (theByte1 * 256) Or (theByte2 * 65536) Or (theByte3 * 16777216)
		End Function
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


	#tag Property, Flags = &h21
		Private pConnection As FlightConnection
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadData As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadFilename As String = ""
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadIsSd As Boolean = True
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadOffset As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDownloadTotal As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pFlashDownloadSlot As Integer = -1
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pFlashDownloadSample As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pFlashDownloadTotal As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pFlashDownloadData As String = ""
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

		  // Get flight info from row tag
		  Var theFlightInfo As Dictionary = ListFlash.RowTagAt(ListFlash.SelectedRowIndex)
		  If theFlightInfo = Nil Then Return

		  Var theSlot As Integer = theFlightInfo.Lookup("slot", 0)
		  Var theSamples As Integer = theFlightInfo.Lookup("samples", 0)

		  LabelFlashStatus.Text = "Downloading slot " + Str(theSlot) + " (" + Str(theSamples) + " samples)..."

		  // Start flash download
		  StartFlashDownload(theSlot, theSamples)
		End Sub
	#tag EndEvent
#tag EndEvents
#tag Events ButtonFlashDelete
	#tag Event
		Sub Action()
		  If ListFlash.SelectedRowIndex < 0 Then Return

		  // Get flight info from row tag
		  Var theFlightInfo As Dictionary = ListFlash.RowTagAt(ListFlash.SelectedRowIndex)
		  If theFlightInfo = Nil Then Return

		  Var theSlot As Integer = theFlightInfo.Lookup("slot", 0)

		  // Confirm deletion
		  Var theResult As Integer = MsgBox("Delete flight in slot " + Str(theSlot) + "?" + EndOfLine + _
		  "This cannot be undone.", 1, "Confirm Delete")

		  If theResult = 1 Then
		    LabelFlashStatus.Text = "Deleting slot " + Str(theSlot) + "..."
		    If pConnection <> Nil And pConnection.IsConnected Then
		      pConnection.SendFlashDelete(theSlot)
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
