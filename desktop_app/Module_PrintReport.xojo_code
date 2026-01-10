#tag Module
Protected Module Module_PrintReport
	#tag Method, Flags = &h1
		Protected Sub RenderFlightReport(g As Graphics, theFlightData As FlightData, theTitle As String = "")
		  // Render a flight report to the given Graphics context
		  // US Letter: 612 x 792 points at 72 dpi

		  Const kMargin = 36 // 0.5 inch margins
		  Const kLineHeight = 14
		  Const kSectionSpacing = 20

		  Var thePageWidth As Double = g.Width
		  Var thePageHeight As Double = g.Height
		  Var theContentWidth As Double = thePageWidth - (2 * kMargin)
		  Var theY As Double = kMargin

		  // Set default font
		  g.FontName = "Helvetica"
		  g.FontSize = 12
		  g.DrawingColor = &c000000

		  // Title
		  g.FontSize = 18
		  g.Bold = True
		  If theTitle = "" Then
		    theTitle = "Rocket Flight Report"
		  End If
		  g.DrawText(theTitle, kMargin, theY + 18)
		  theY = theY + 24 + kSectionSpacing

		  // Flight Information Section
		  g.FontSize = 14
		  g.Bold = True
		  g.DrawText("Flight Information", kMargin, theY + 14)
		  theY = theY + 20

		  g.FontSize = 10
		  g.Bold = False

		  // Two-column layout for flight info
		  Var theCol1X As Double = kMargin
		  Var theCol2X As Double = kMargin + theContentWidth / 2

		  // Column 1
		  g.Bold = True
		  g.DrawText("Rocket :", theCol1X, theY + kLineHeight)
		  g.Bold = False
		  g.DrawText(theFlightData.pRocketName, theCol1X + 60, theY + kLineHeight)
		  theY = theY + kLineHeight

		  g.Bold = True
		  g.DrawText("Date :", theCol1X, theY + kLineHeight)
		  g.Bold = False
		  If theFlightData.pTimestamp <> Nil Then
		    g.DrawText(theFlightData.pTimestamp.ToString, theCol1X + 60, theY + kLineHeight)
		  End If
		  theY = theY + kLineHeight

		  g.Bold = True
		  g.DrawText("Pilot :", theCol1X, theY + kLineHeight)
		  g.Bold = False
		  g.DrawText(theFlightData.pPilotName, theCol1X + 60, theY + kLineHeight)
		  theY = theY + kLineHeight

		  g.Bold = True
		  g.DrawText("Motor :", theCol1X, theY + kLineHeight)
		  g.Bold = False
		  g.DrawText(theFlightData.pMotorDesignation, theCol1X + 60, theY + kLineHeight)
		  theY = theY + kLineHeight

		  g.Bold = True
		  g.DrawText("Location :", theCol1X, theY + kLineHeight)
		  g.Bold = False
		  g.DrawText(theFlightData.pLocation, theCol1X + 60, theY + kLineHeight)
		  theY = theY + kLineHeight + kSectionSpacing

		  // Flight Results Section
		  g.FontSize = 14
		  g.Bold = True
		  g.DrawText("Flight Results", kMargin, theY + 14)
		  theY = theY + 20

		  g.FontSize = 10
		  g.Bold = False

		  // Get results from FlightData properties
		  Var theMaxAltitude As Double = theFlightData.pMaxAltitudeM
		  Var theMaxVelocity As Double = theFlightData.pMaxVelocityMps
		  Var theApogeeTime As Double = theFlightData.GetApogeeTimeSeconds
		  Var theFlightTime As Double = theFlightData.GetFlightTimeSeconds

		  g.Bold = True
		  g.DrawText("Max Altitude :", theCol1X, theY + kLineHeight)
		  g.Bold = False
		  g.DrawText(Format(theMaxAltitude, "0.0") + " m (" + Format(theMaxAltitude * 3.28084, "0.0") + " ft)", theCol1X + 90, theY + kLineHeight)

		  g.Bold = True
		  g.DrawText("Max Velocity :", theCol2X, theY + kLineHeight)
		  g.Bold = False
		  g.DrawText(Format(theMaxVelocity, "0.0") + " m/s", theCol2X + 90, theY + kLineHeight)
		  theY = theY + kLineHeight

		  g.Bold = True
		  g.DrawText("Apogee Time :", theCol1X, theY + kLineHeight)
		  g.Bold = False
		  g.DrawText(Format(theApogeeTime, "0.00") + " s", theCol1X + 90, theY + kLineHeight)

		  g.Bold = True
		  g.DrawText("Flight Time :", theCol2X, theY + kLineHeight)
		  g.Bold = False
		  g.DrawText(Format(theFlightTime, "0.00") + " s", theCol2X + 90, theY + kLineHeight)
		  theY = theY + kLineHeight

		  g.Bold = True
		  g.DrawText("Samples :", theCol1X, theY + kLineHeight)
		  g.Bold = False
		  g.DrawText(Str(theFlightData.GetSampleCount), theCol1X + 90, theY + kLineHeight)
		  theY = theY + kLineHeight + kSectionSpacing

		  // Notes Section (if any)
		  If theFlightData.pNotes <> "" Then
		    g.FontSize = 14
		    g.Bold = True
		    g.DrawText("Notes", kMargin, theY + 14)
		    theY = theY + 20

		    g.FontSize = 10
		    g.Bold = False
		    // Simple word wrap for notes
		    Var theNotes As String = theFlightData.pNotes
		    Var theLines() As String = theNotes.Split(EndOfLine)
		    For Each theLine As String In theLines
		      g.DrawText(theLine, kMargin, theY + kLineHeight)
		      theY = theY + kLineHeight
		    Next
		    theY = theY + kSectionSpacing
		  End If

		  // Altitude Curve Section
		  g.FontSize = 14
		  g.Bold = True
		  g.DrawText("Altitude Profile", kMargin, theY + 14)
		  theY = theY + 20

		  // Calculate chart dimensions
		  Var theChartHeight As Double = thePageHeight - theY - kMargin - 20
		  If theChartHeight > 350 Then theChartHeight = 350
		  Var theChartWidth As Double = theContentWidth

		  // Draw chart
		  RenderAltitudeChart(g, theFlightData, kMargin, theY, theChartWidth, theChartHeight)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Sub RenderAltitudeChart(g As Graphics, theFlightData As FlightData, theX As Double, theY As Double, theWidth As Double, theHeight As Double)
		  // Render an altitude chart at the specified location
		  Const kPadding = 40

		  // Chart area
		  Var theChartLeft As Double = theX + kPadding
		  Var theChartTop As Double = theY + 10
		  Var theChartWidth As Double = theWidth - kPadding - 20
		  Var theChartHeight As Double = theHeight - kPadding - 10

		  // Draw border
		  g.DrawingColor = &c888888
		  g.DrawRectangle(theChartLeft, theChartTop, theChartWidth, theChartHeight)

		  // Get data range
		  Var theMaxTime As Double = theFlightData.GetFlightTimeSeconds
		  Var theMaxAltitude As Double = theFlightData.pMaxAltitudeM

		  If theMaxTime <= 0 Then theMaxTime = 1
		  If theMaxAltitude <= 0 Then theMaxAltitude = 1

		  // Add 10% padding to max values
		  theMaxTime = theMaxTime * 1.1
		  theMaxAltitude = theMaxAltitude * 1.1

		  // Round up to nice values
		  theMaxAltitude = Ceiling(theMaxAltitude / 50) * 50
		  theMaxTime = Ceiling(theMaxTime * 2) / 2

		  // Draw grid lines
		  g.DrawingColor = &cE0E0E0
		  g.FontSize = 8
		  g.Bold = False

		  // Horizontal grid lines (altitude)
		  Var theAltStep As Double = theMaxAltitude / 5
		  For i As Integer = 0 To 5
		    Var theGridY As Double = theChartTop + theChartHeight - (i * theChartHeight / 5)
		    g.DrawingColor = &cE0E0E0
		    g.DrawLine(theChartLeft, theGridY, theChartLeft + theChartWidth, theGridY)
		    g.DrawingColor = &c666666
		    g.DrawText(Format(i * theAltStep, "0"), theX + 5, theGridY + 3)
		  Next

		  // Vertical grid lines (time)
		  Var theTimeStep As Double = theMaxTime / 5
		  For i As Integer = 0 To 5
		    Var theGridX As Double = theChartLeft + (i * theChartWidth / 5)
		    g.DrawingColor = &cE0E0E0
		    g.DrawLine(theGridX, theChartTop, theGridX, theChartTop + theChartHeight)
		    g.DrawingColor = &c666666
		    g.DrawText(Format(i * theTimeStep, "0.0"), theGridX - 10, theY + theHeight - 5)
		  Next

		  // Draw axis labels
		  g.DrawingColor = &c000000
		  g.FontSize = 9
		  g.DrawText("Altitude (m)", theX, theChartTop + theChartHeight / 2)
		  g.DrawText("Time (s)", theChartLeft + theChartWidth / 2 - 20, theY + theHeight)

		  // Draw altitude curve
		  g.DrawingColor = &c0066CC
		  g.PenSize = 2

		  Var theSamples() As TelemetrySample = theFlightData.pSamples
		  If theSamples.Count > 1 Then
		    Var theLastX As Double = theChartLeft
		    Var theLastY As Double = theChartTop + theChartHeight

		    For i As Integer = 0 To theSamples.LastIndex
		      Var theSample As TelemetrySample = theSamples(i)
		      Var thePlotX As Double = theChartLeft + (theSample.GetTimeSeconds / theMaxTime) * theChartWidth
		      Var thePlotY As Double = theChartTop + theChartHeight - (theSample.pAltitudeM / theMaxAltitude) * theChartHeight

		      If i > 0 Then
		        g.DrawLine(theLastX, theLastY, thePlotX, thePlotY)
		      End If

		      theLastX = thePlotX
		      theLastY = thePlotY
		    Next
		  End If

		  g.PenSize = 1
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Sub RenderMultiFlightReport(g As Graphics, theFlights() As FlightData)
		  // Render a report with multiple flight curves overlaid
		  // US Letter: 612 x 792 points at 72 dpi

		  Const kMargin = 36
		  Const kLineHeight = 14
		  Const kSectionSpacing = 20

		  Var thePageWidth As Double = g.Width
		  Var thePageHeight As Double = g.Height
		  Var theContentWidth As Double = thePageWidth - (2 * kMargin)
		  Var theY As Double = kMargin

		  // Set default font
		  g.FontName = "Helvetica"
		  g.FontSize = 12
		  g.DrawingColor = &c000000

		  // Title
		  g.FontSize = 18
		  g.Bold = True
		  g.DrawText("Flight Comparison Report", kMargin, theY + 18)
		  theY = theY + 24 + kSectionSpacing

		  // Flight list
		  g.FontSize = 14
		  g.Bold = True
		  g.DrawText("Flights Included", kMargin, theY + 14)
		  theY = theY + 20

		  g.FontSize = 10
		  g.Bold = False

		  // Color palette for curves
		  Var theColors() As Color = Array(&c0066CC, &cCC0000, &c00CC00, &cCC6600, &c6600CC, &c00CCCC, &cCC0066, &c66CC00)

		  For i As Integer = 0 To theFlights.LastIndex
		    Var theFlight As FlightData = theFlights(i)
		    Var theColor As Color = theColors(i Mod theColors.Count)

		    // Draw color swatch
		    g.DrawingColor = theColor
		    g.FillRectangle(kMargin, theY + 3, 12, 12)
		    g.DrawingColor = &c000000
		    g.DrawRectangle(kMargin, theY + 3, 12, 12)

		    // Flight info
		    Var theInfo As String = theFlight.pRocketName
		    If theInfo = "" Then theInfo = "Flight " + Str(i + 1)
		    If theFlight.pTimestamp <> Nil Then
		      theInfo = theInfo + " - " + theFlight.pTimestamp.ToString
		    End If
		    theInfo = theInfo + " (Max Alt: " + Format(theFlight.pMaxAltitudeM, "0.0") + " m)"
		    g.DrawText(theInfo, kMargin + 20, theY + kLineHeight)
		    theY = theY + kLineHeight + 2
		  Next

		  theY = theY + kSectionSpacing

		  // Altitude Curves Section
		  g.FontSize = 14
		  g.Bold = True
		  g.DrawText("Altitude Profiles", kMargin, theY + 14)
		  theY = theY + 20

		  // Calculate chart dimensions
		  Var theChartHeight As Double = thePageHeight - theY - kMargin - 20
		  If theChartHeight > 400 Then theChartHeight = 400
		  Var theChartWidth As Double = theContentWidth

		  // Draw multi-curve chart
		  RenderMultiAltitudeChart(g, theFlights, theColors, kMargin, theY, theChartWidth, theChartHeight)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Sub RenderMultiAltitudeChart(g As Graphics, theFlights() As FlightData, theColors() As Color, theX As Double, theY As Double, theWidth As Double, theHeight As Double)
		  // Render multiple altitude curves overlaid
		  Const kPadding = 40

		  // Chart area
		  Var theChartLeft As Double = theX + kPadding
		  Var theChartTop As Double = theY + 10
		  Var theChartWidth As Double = theWidth - kPadding - 20
		  Var theChartHeight As Double = theHeight - kPadding - 10

		  // Draw border
		  g.DrawingColor = &c888888
		  g.DrawRectangle(theChartLeft, theChartTop, theChartWidth, theChartHeight)

		  // Find max values across all flights
		  Var theMaxTime As Double = 0
		  Var theMaxAltitude As Double = 0
		  For Each theFlight As FlightData In theFlights
		    If theFlight.GetFlightTimeSeconds > theMaxTime Then theMaxTime = theFlight.GetFlightTimeSeconds
		    If theFlight.pMaxAltitudeM > theMaxAltitude Then theMaxAltitude = theFlight.pMaxAltitudeM
		  Next

		  If theMaxTime <= 0 Then theMaxTime = 1
		  If theMaxAltitude <= 0 Then theMaxAltitude = 1

		  // Add 10% padding
		  theMaxTime = theMaxTime * 1.1
		  theMaxAltitude = theMaxAltitude * 1.1

		  // Round up to nice values
		  theMaxAltitude = Ceiling(theMaxAltitude / 50) * 50
		  theMaxTime = Ceiling(theMaxTime * 2) / 2

		  // Draw grid lines
		  g.FontSize = 8
		  g.Bold = False

		  // Horizontal grid lines
		  Var theAltStep As Double = theMaxAltitude / 5
		  For i As Integer = 0 To 5
		    Var theGridY As Double = theChartTop + theChartHeight - (i * theChartHeight / 5)
		    g.DrawingColor = &cE0E0E0
		    g.DrawLine(theChartLeft, theGridY, theChartLeft + theChartWidth, theGridY)
		    g.DrawingColor = &c666666
		    g.DrawText(Format(i * theAltStep, "0"), theX + 5, theGridY + 3)
		  Next

		  // Vertical grid lines
		  Var theTimeStep As Double = theMaxTime / 5
		  For i As Integer = 0 To 5
		    Var theGridX As Double = theChartLeft + (i * theChartWidth / 5)
		    g.DrawingColor = &cE0E0E0
		    g.DrawLine(theGridX, theChartTop, theGridX, theChartTop + theChartHeight)
		    g.DrawingColor = &c666666
		    g.DrawText(Format(i * theTimeStep, "0.0"), theGridX - 10, theY + theHeight - 5)
		  Next

		  // Draw axis labels
		  g.DrawingColor = &c000000
		  g.FontSize = 9
		  g.DrawText("Altitude (m)", theX, theChartTop + theChartHeight / 2)
		  g.DrawText("Time (s)", theChartLeft + theChartWidth / 2 - 20, theY + theHeight)

		  // Draw each curve
		  For i As Integer = 0 To theFlights.LastIndex
		    Var theFlight As FlightData = theFlights(i)
		    Var theColor As Color = theColors(i Mod theColors.Count)

		    g.DrawingColor = theColor
		    g.PenSize = 2

		    Var theSamples() As TelemetrySample = theFlight.pSamples
		    If theSamples.Count > 1 Then
		      Var theLastX As Double = theChartLeft
		      Var theLastY As Double = theChartTop + theChartHeight

		      For j As Integer = 0 To theSamples.LastIndex
		        Var theSample As TelemetrySample = theSamples(j)
		        Var thePlotX As Double = theChartLeft + (theSample.GetTimeSeconds / theMaxTime) * theChartWidth
		        Var thePlotY As Double = theChartTop + theChartHeight - (theSample.pAltitudeM / theMaxAltitude) * theChartHeight

		        If j > 0 Then
		          g.DrawLine(theLastX, theLastY, thePlotX, thePlotY)
		        End If

		        theLastX = thePlotX
		        theLastY = thePlotY
		      Next
		    End If
		  Next

		  g.PenSize = 1
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Function ExportPDF(theFlightData As FlightData) As FolderItem
		  // Export a single flight to PDF using vector graphics
		  // Returns the file or Nil if cancelled

		  Var theDlg As New SaveAsDialog
		  theDlg.Title = "Export PDF"
		  theDlg.SuggestedFileName = "flight_report.pdf"

		  Var theFile As FolderItem = theDlg.ShowModal
		  If theFile = Nil Then Return Nil

		  // US Letter size at 72 dpi
		  Const kPageWidth = 612
		  Const kPageHeight = 792

		  // Create PDF document with vector graphics
		  Var thePDF As New PDFDocument(kPageWidth, kPageHeight)
		  Var g As Graphics = thePDF.Graphics

		  // Render the report to the PDF graphics context
		  RenderFlightReport(g, theFlightData)

		  // Save the PDF
		  thePDF.Save(theFile)

		  Return theFile
		End Function
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Function ExportMultiPDF(theFlights() As FlightData) As FolderItem
		  // Export multiple flights to PDF using vector graphics
		  // Returns the file or Nil if cancelled

		  Var theDlg As New SaveAsDialog
		  theDlg.Title = "Export PDF"
		  theDlg.SuggestedFileName = "flight_comparison.pdf"

		  Var theFile As FolderItem = theDlg.ShowModal
		  If theFile = Nil Then Return Nil

		  // US Letter size at 72 dpi
		  Const kPageWidth = 612
		  Const kPageHeight = 792

		  // Create PDF document with vector graphics
		  Var thePDF As New PDFDocument(kPageWidth, kPageHeight)
		  Var g As Graphics = thePDF.Graphics

		  // Render the multi-flight report to the PDF graphics context
		  RenderMultiFlightReport(g, theFlights)

		  // Save the PDF
		  thePDF.Save(theFile)

		  Return theFile
		End Function
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Sub PrintFlight(theFlightData As FlightData)
		  // Print a single flight report using vector graphics

		  Var ps As PrinterSetup
		  If pPrinterSetup <> Nil Then
		    ps = pPrinterSetup
		  Else
		    ps = New PrinterSetup
		  End If

		  Var g As Graphics = OpenPrinterDialog(ps)
		  If g = Nil Then Return

		  RenderFlightReport(g, theFlightData)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Sub PrintMultiFlight(theFlights() As FlightData)
		  // Print a multi-flight comparison report using vector graphics

		  Var ps As PrinterSetup
		  If pPrinterSetup <> Nil Then
		    ps = pPrinterSetup
		  Else
		    ps = New PrinterSetup
		  End If

		  Var g As Graphics = OpenPrinterDialog(ps)
		  If g = Nil Then Return

		  RenderMultiFlightReport(g, theFlights)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h1
		Protected Sub ShowPageSetup()
		  // Show the page setup dialog and store settings
		  If pPrinterSetup = Nil Then
		    pPrinterSetup = New PrinterSetup
		  End If
		  Call pPrinterSetup.PageSetupDialog
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pPrinterSetup As PrinterSetup
	#tag EndProperty


End Module
#tag EndModule
