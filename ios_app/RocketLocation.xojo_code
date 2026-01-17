#tag Class
Protected Class RocketLocation
	#tag Method, Flags = &h0
		Sub Constructor()
		  // Initialize with default values
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
		Shared Function Load() As RocketLocation
		  // Load persisted rocket location from Preferences
		  Var theLoc As New RocketLocation

		  theLoc.pLatitude = Preferences.DoubleValue("rocket_lat", 0.0)
		  theLoc.pLongitude = Preferences.DoubleValue("rocket_lon", 0.0)
		  theLoc.pAltitude = Preferences.DoubleValue("rocket_alt", 0.0)
		  theLoc.pGpsFix = Preferences.BooleanValue("rocket_fix", False)
		  theLoc.pSatellites = Preferences.IntegerValue("rocket_sats", 0)
		  theLoc.pFlightState = Preferences.StringValue("rocket_state", "")

		  // Load timestamp
		  Var theTimestampStr As String = Preferences.StringValue("rocket_timestamp", "")
		  If theTimestampStr <> "" Then
		    Try
		      theLoc.pTimestamp = DateTime.FromString(theTimestampStr, Locale.Current, TimeZone.Current)
		    Catch
		      theLoc.pTimestamp = DateTime.Now
		    End Try
		  Else
		    theLoc.pTimestamp = DateTime.Now
		  End If

		  Return theLoc
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Save()
		  // Save rocket location to Preferences
		  Preferences.DoubleValue("rocket_lat") = pLatitude
		  Preferences.DoubleValue("rocket_lon") = pLongitude
		  Preferences.DoubleValue("rocket_alt") = pAltitude
		  Preferences.BooleanValue("rocket_fix") = pGpsFix
		  Preferences.IntegerValue("rocket_sats") = pSatellites
		  Preferences.StringValue("rocket_state") = pFlightState
		  Preferences.StringValue("rocket_timestamp") = pTimestamp.ToString(Locale.Current, TimeZone.Current)
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
