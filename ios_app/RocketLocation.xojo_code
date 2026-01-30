#tag Class
Protected Class RocketLocation
	#tag Method, Flags = &h0
		Sub Constructor()
		  // Initialize with default values
		  pRocketId = 0
		  pLatitude = 0.0
		  pLongitude = 0.0
		  pAltitude = 0.0
		  pTimestamp = DateTime.Now
		  pGpsFix = False
		  pSatellites = 0
		  pFlightState = ""
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Function Load(inRocketId As Integer = 0) As RocketLocation
		  // Load persisted rocket location from Preferences
		  // Supports multiple rockets via ID-based keys
		  Var theLoc As New RocketLocation

		  Try
		    Var thePrefix As String = "rocket_" + Str(inRocketId) + "_"

		    theLoc.pRocketId = inRocketId
		    theLoc.pLatitude = App.GetPreferenceDouble(thePrefix + "lat", 0.0)
		    theLoc.pLongitude = App.GetPreferenceDouble(thePrefix + "lon", 0.0)
		    theLoc.pAltitude = App.GetPreferenceDouble(thePrefix + "alt", 0.0)
		    theLoc.pGpsFix = App.GetPreferenceBoolean(thePrefix + "fix", False)
		    theLoc.pSatellites = App.GetPreferenceInteger(thePrefix + "sats", 0)
		    theLoc.pFlightState = App.GetPreferenceString(thePrefix + "state", "")

		    // Load timestamp
		    Var theTimestampStr As String = App.GetPreferenceString(thePrefix + "timestamp", "")
		    If theTimestampStr <> "" Then
		      Try
		        theLoc.pTimestamp = DateTime.FromString(theTimestampStr, Locale.Current, TimeZone.Current)
		      Catch
		        theLoc.pTimestamp = DateTime.Now
		      End Try
		    Else
		      theLoc.pTimestamp = DateTime.Now
		    End If
		  Catch e As RuntimeException
		    // Return default values on error
		    theLoc.pTimestamp = DateTime.Now
		  End Try

		  Return theLoc
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Save()
		  // Save rocket location to Preferences
		  // Uses ID-based keys for multi-rocket support
		  Var thePrefix As String = "rocket_" + Str(pRocketId) + "_"

		  App.SetPreferenceDouble(thePrefix + "lat", pLatitude)
		  App.SetPreferenceDouble(thePrefix + "lon", pLongitude)
		  App.SetPreferenceDouble(thePrefix + "alt", pAltitude)
		  App.SetPreferenceBoolean(thePrefix + "fix", pGpsFix)
		  App.SetPreferenceInteger(thePrefix + "sats", pSatellites)
		  App.SetPreferenceString(thePrefix + "state", pFlightState)
		  App.SetPreferenceString(thePrefix + "timestamp", pTimestamp.ToString)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function IsValid() As Boolean
		  // Check if we have valid coordinates
		  Return pGpsFix And (pLatitude <> 0.0 Or pLongitude <> 0.0)
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetAgeSeconds() As Integer
		  // Return age of location in seconds
		  If pTimestamp = Nil Then Return 999999

		  Return DateTime.Now.SecondsFrom1970 - pTimestamp.SecondsFrom1970
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetAgeString() As String
		  // Return human-readable age string
		  Var theAge As Integer = GetAgeSeconds()

		  If theAge < 60 Then
		    Return Str(theAge) + " sec ago"
		  ElseIf theAge < 3600 Then
		    Return Str(theAge \ 60) + " min ago"
		  ElseIf theAge < 86400 Then
		    Return Str(theAge \ 3600) + " hr ago"
		  Else
		    Return Str(theAge \ 86400) + " days ago"
		  End If
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub UpdateFromTelemetry(inSample As TelemetrySample)
		  // Update rocket location from telemetry sample
		  If inSample.pGpsFix Then
		    pLatitude = inSample.pGpsLatitude
		    pLongitude = inSample.pGpsLongitude
		    pAltitude = inSample.pAltitudeM
		    pGpsFix = True
		    pSatellites = inSample.pGpsSatellites
		    pFlightState = inSample.pState
		    pTimestamp = DateTime.Now
		  End If
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h0
		pRocketId As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pLatitude As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pLongitude As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pAltitude As Double = 0.0
	#tag EndProperty

	#tag Property, Flags = &h0
		pTimestamp As DateTime
	#tag EndProperty

	#tag Property, Flags = &h0
		pGpsFix As Boolean = False
	#tag EndProperty

	#tag Property, Flags = &h0
		pSatellites As Integer = 0
	#tag EndProperty

	#tag Property, Flags = &h0
		pFlightState As String = ""
	#tag EndProperty


End Class
#tag EndClass
