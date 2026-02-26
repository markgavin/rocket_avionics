#tag Class
Protected Class FlightData
	#tag Method, Flags = &h0
		Sub AddSample(inSample As TelemetrySample)
		  // Add a telemetry sample to the flight
		  pSamples.Add(inSample)

		  // Update max values
		  If inSample.pAltitudeM > pMaxAltitudeM Then
		    pMaxAltitudeM = inSample.pAltitudeM
		    pApogeeTimeMs = inSample.pTimeMs
		  End If

		  If inSample.pVelocityMps > pMaxVelocityMps Then
		    pMaxVelocityMps = inSample.pVelocityMps
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Constructor()
		  // Initialize with default values
		  pFlightId = GenerateUUID()
		  pTimestamp = DateTime.Now
		  ReDim pSamples(-1)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Function GenerateUUID() As String
		  // Generate unique identifier
		  Var theRandom As New Random
		  Var theHex As String = "0123456789abcdef"
		  Var theUuid As String = ""

		  For theIndex As Integer = 1 To 32
		    theUuid = theUuid + theHex.Middle(theRandom.InRange(0, 15), 1)
		    If theIndex = 8 Or theIndex = 12 Or theIndex = 16 Or theIndex = 20 Then
		      theUuid = theUuid + "-"
		    End If
		  Next

		  Return theUuid
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetFlightTimeSeconds() As Double
		  // Get total flight time in seconds
		  Return pFlightTimeMs / 1000.0
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetApogeeTimeSeconds() As Double
		  // Get time to apogee in seconds
		  Return pApogeeTimeMs / 1000.0
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetSampleCount() As Integer
		  // Return number of samples
		  Return pSamples.Count
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Function FromJSON(inJSON As String) As FlightData
		  // Parse JSON data into FlightData object
		  Try
		    Var theJson As New JSONItem(inJSON)
		    Var theData As New FlightData

		    If theJson.HasKey("flight_id") Then
		      theData.pFlightId = theJson.Value("flight_id")
		    End If
		    If theJson.HasKey("pilot_name") Then
		      theData.pPilotName = theJson.Value("pilot_name")
		    End If
		    If theJson.HasKey("rocket_name") Then
		      theData.pRocketName = theJson.Value("rocket_name")
		    End If
		    If theJson.HasKey("motor_designation") Then
		      theData.pMotorDesignation = theJson.Value("motor_designation")
		    End If
		    If theJson.HasKey("location") Then
		      theData.pLocation = theJson.Value("location")
		    End If
		    If theJson.HasKey("notes") Then
		      theData.pNotes = theJson.Value("notes")
		    End If
		    If theJson.HasKey("max_altitude_m") Then
		      theData.pMaxAltitudeM = theJson.Value("max_altitude_m")
		    End If
		    If theJson.HasKey("max_velocity_mps") Then
		      theData.pMaxVelocityMps = theJson.Value("max_velocity_mps")
		    End If
		    If theJson.HasKey("apogee_time_ms") Then
		      theData.pApogeeTimeMs = theJson.Value("apogee_time_ms")
		    End If
		    If theJson.HasKey("flight_time_ms") Then
		      theData.pFlightTimeMs = theJson.Value("flight_time_ms")
		    End If

		    // Parse samples
		    If theJson.HasKey("samples") Then
		      Var theSamplesArray As JSONItem = theJson.Value("samples")
		      For theSampleIndex As Integer = 0 To theSamplesArray.Count - 1
		        Var theSampleJson As JSONItem = theSamplesArray.ChildAt(theSampleIndex)
		        Var theSample As New TelemetrySample
		        theSample.pTimeMs = theSampleJson.Lookup("t_ms", 0)
		        theSample.pAltitudeM = theSampleJson.Lookup("alt", 0.0)
		        theSample.pVelocityMps = theSampleJson.Lookup("vel", 0.0)
		        theSample.pPressurePa = theSampleJson.Lookup("pres", 0.0)
		        theSample.pTemperatureC = theSampleJson.Lookup("temp", 0.0)
		        theSample.pState = theSampleJson.Lookup("state", "")

		        // GPS data
		        theSample.pGpsLatitude = theSampleJson.Lookup("lat", 0.0)
		        theSample.pGpsLongitude = theSampleJson.Lookup("lon", 0.0)
		        theSample.pGpsSpeedMps = theSampleJson.Lookup("gspd", 0.0)
		        theSample.pGpsHeadingDeg = theSampleJson.Lookup("hdg", 0.0)
		        theSample.pGpsSatellites = theSampleJson.Lookup("sats", 0)
		        theSample.pGpsFix = (theSample.pGpsSatellites >= 1)

		        // IMU data
		        theSample.pAccelX = theSampleJson.Lookup("ax", 0)
		        theSample.pAccelY = theSampleJson.Lookup("ay", 0)
		        theSample.pAccelZ = theSampleJson.Lookup("az", 0)
		        theSample.pGyroX = theSampleJson.Lookup("gx", 0.0)
		        theSample.pGyroY = theSampleJson.Lookup("gy", 0.0)
		        theSample.pGyroZ = theSampleJson.Lookup("gz", 0.0)
		        theSample.pMagX = theSampleJson.Lookup("mx", 0.0)
		        theSample.pMagY = theSampleJson.Lookup("my", 0.0)
		        theSample.pMagZ = theSampleJson.Lookup("mz", 0.0)

		        theData.pSamples.Add(theSample)
		      Next
		    End If

		    Return theData

		  Catch theError As JSONException
		    Return Nil
		  End Try
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function ToCSV() As String
		  // Export flight data as CSV string
		  Var theLines() As String

		  // Header comments
		  theLines.Add("# Rocket Flight Data")
		  theLines.Add("# Flight ID: " + pFlightId)
		  theLines.Add("# Date: " + pTimestamp.ToString)
		  theLines.Add("# Pilot: " + pPilotName)
		  theLines.Add("# Rocket: " + pRocketName)
		  theLines.Add("# Motor: " + pMotorDesignation)
		  theLines.Add("# Location: " + pLocation)
		  theLines.Add("# Max Altitude (m): " + Format(pMaxAltitudeM, "0.00"))
		  theLines.Add("# Max Velocity (m/s): " + Format(pMaxVelocityMps, "0.00"))
		  theLines.Add("# Apogee Time (s): " + Format(GetApogeeTimeSeconds(), "0.000"))
		  theLines.Add("# Flight Time (s): " + Format(GetFlightTimeSeconds(), "0.000"))
		  theLines.Add("# Notes: " + pNotes)
		  theLines.Add("#")
		  theLines.Add("Time_ms,Altitude_m,Velocity_mps,Pressure_Pa,Temperature_C,State," + _
		    "GPS_Lat,GPS_Lon,GPS_Speed_mps,GPS_Hdg_deg,GPS_Sat,GPS_Fix," + _
		    "Accel_X,Accel_Y,Accel_Z,Gyro_X,Gyro_Y,Gyro_Z,Mag_X,Mag_Y,Mag_Z")

		  // Data rows
		  For Each theSample As TelemetrySample In pSamples
		    theLines.Add(Str(theSample.pTimeMs) + "," + _
		      Format(theSample.pAltitudeM, "0.00") + "," + _
		      Format(theSample.pVelocityMps, "0.00") + "," + _
		      Format(theSample.pPressurePa, "0.0") + "," + _
		      Format(theSample.pTemperatureC, "0.0") + "," + _
		      theSample.pState + "," + _
		      Format(theSample.pGpsLatitude, "0.000000") + "," + _
		      Format(theSample.pGpsLongitude, "0.000000") + "," + _
		      Format(theSample.pGpsSpeedMps, "0.00") + "," + _
		      Format(theSample.pGpsHeadingDeg, "0.0") + "," + _
		      Str(theSample.pGpsSatellites) + "," + _
		      If(theSample.pGpsFix, "1", "0") + "," + _
		      Str(theSample.pAccelX) + "," + _
		      Str(theSample.pAccelY) + "," + _
		      Str(theSample.pAccelZ) + "," + _
		      Format(theSample.pGyroX, "0.00") + "," + _
		      Format(theSample.pGyroY, "0.00") + "," + _
		      Format(theSample.pGyroZ, "0.00") + "," + _
		      Format(theSample.pMagX, "0.00") + "," + _
		      Format(theSample.pMagY, "0.00") + "," + _
		      Format(theSample.pMagZ, "0.00"))
		  Next

		  Return String.FromArray(theLines, EndOfLine)
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function ToJSON() As String
		  // Export flight data as JSON string
		  Var theJson As New JSONItem

		  theJson.Value("flight_id") = pFlightId
		  theJson.Value("timestamp") = pTimestamp.ToString
		  theJson.Value("pilot_name") = pPilotName
		  theJson.Value("rocket_name") = pRocketName
		  theJson.Value("motor_designation") = pMotorDesignation
		  theJson.Value("location") = pLocation
		  theJson.Value("notes") = pNotes
		  theJson.Value("max_altitude_m") = pMaxAltitudeM
		  theJson.Value("max_velocity_mps") = pMaxVelocityMps
		  theJson.Value("apogee_time_ms") = pApogeeTimeMs
		  theJson.Value("flight_time_ms") = pFlightTimeMs

		  Var theSamplesArray As New JSONItem
		  For Each theSample As TelemetrySample In pSamples
		    Var theSampleJson As New JSONItem
		    theSampleJson.Value("t_ms") = theSample.pTimeMs
		    theSampleJson.Value("alt") = theSample.pAltitudeM
		    theSampleJson.Value("vel") = theSample.pVelocityMps
		    theSampleJson.Value("pres") = theSample.pPressurePa
		    theSampleJson.Value("temp") = theSample.pTemperatureC
		    theSampleJson.Value("state") = theSample.pState

		    // GPS data
		    theSampleJson.Value("lat") = theSample.pGpsLatitude
		    theSampleJson.Value("lon") = theSample.pGpsLongitude
		    theSampleJson.Value("gspd") = theSample.pGpsSpeedMps
		    theSampleJson.Value("hdg") = theSample.pGpsHeadingDeg
		    theSampleJson.Value("sat") = theSample.pGpsSatellites
		    theSampleJson.Value("gps") = theSample.pGpsFix

		    // IMU data
		    theSampleJson.Value("ax") = theSample.pAccelX
		    theSampleJson.Value("ay") = theSample.pAccelY
		    theSampleJson.Value("az") = theSample.pAccelZ
		    theSampleJson.Value("gx") = theSample.pGyroX
		    theSampleJson.Value("gy") = theSample.pGyroY
		    theSampleJson.Value("gz") = theSample.pGyroZ
		    theSampleJson.Value("mx") = theSample.pMagX
		    theSampleJson.Value("my") = theSample.pMagY
		    theSampleJson.Value("mz") = theSample.pMagZ

		    theSamplesArray.Add(theSampleJson)
		  Next
		  theJson.Value("samples") = theSamplesArray

		  Return theJson.ToString
		End Function
	#tag EndMethod


	#tag Property, Flags = &h0
		pFlightId As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		pTimestamp As DateTime
	#tag EndProperty

	#tag Property, Flags = &h0
		pPilotName As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		pRocketName As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		pMotorDesignation As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		pLocation As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		pNotes As String = ""
	#tag EndProperty

	#tag Property, Flags = &h0
		pMaxAltitudeM As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pMaxVelocityMps As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pApogeeTimeMs As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pFlightTimeMs As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pSamples() As TelemetrySample
	#tag EndProperty


End Class
#tag EndClass
