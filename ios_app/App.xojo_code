#tag Class
Protected Class App
Inherits MobileApplication
	#tag Event
		Sub Opening()
		  // Initialize preferences
		  pPrefs = New Dictionary

		  // Initialize connection handler
		  pConnection = New FlightConnection

		  // Load persisted rocket location (for offline recovery)
		  pRocketLocation = RocketLocation.Load(0)
		End Sub
	#tag EndEvent

	#tag Event
		Sub Closing()
		  // Clean up connection
		  If pConnection <> Nil Then
		    pConnection.Disconnect
		  End If
		End Sub
	#tag EndEvent


	#tag Method, Flags = &h0
		Shared Function GetPreferenceString(inKey As String, inDefault As String) As String
		  If pPrefs = Nil Then pPrefs = New Dictionary
		  If pPrefs.HasKey(inKey) Then
		    Return pPrefs.Value(inKey).StringValue
		  End If
		  Return inDefault
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Sub SetPreferenceString(inKey As String, inValue As String)
		  If pPrefs = Nil Then pPrefs = New Dictionary
		  pPrefs.Value(inKey) = inValue
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Function GetPreferenceDouble(inKey As String, inDefault As Double) As Double
		  If pPrefs = Nil Then pPrefs = New Dictionary
		  If pPrefs.HasKey(inKey) Then
		    Return pPrefs.Value(inKey).DoubleValue
		  End If
		  Return inDefault
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Sub SetPreferenceDouble(inKey As String, inValue As Double)
		  If pPrefs = Nil Then pPrefs = New Dictionary
		  pPrefs.Value(inKey) = inValue
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Function GetPreferenceInteger(inKey As String, inDefault As Integer) As Integer
		  If pPrefs = Nil Then pPrefs = New Dictionary
		  If pPrefs.HasKey(inKey) Then
		    Return pPrefs.Value(inKey).IntegerValue
		  End If
		  Return inDefault
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Sub SetPreferenceInteger(inKey As String, inValue As Integer)
		  If pPrefs = Nil Then pPrefs = New Dictionary
		  pPrefs.Value(inKey) = inValue
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Function GetPreferenceBoolean(inKey As String, inDefault As Boolean) As Boolean
		  If pPrefs = Nil Then pPrefs = New Dictionary
		  If pPrefs.HasKey(inKey) Then
		    Return pPrefs.Value(inKey).BooleanValue
		  End If
		  Return inDefault
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Shared Sub SetPreferenceBoolean(inKey As String, inValue As Boolean)
		  If pPrefs = Nil Then pPrefs = New Dictionary
		  pPrefs.Value(inKey) = inValue
		End Sub
	#tag EndMethod


	#tag Property, Flags = &h0
		pConnection As FlightConnection
	#tag EndProperty

	#tag Property, Flags = &h0
		pRocketLocation As RocketLocation
	#tag EndProperty

	#tag Property, Flags = &h0
		pSelectedRocketId As Integer = -1
	#tag EndProperty

	#tag Property, Flags = &h0
		Shared pPrefs As Dictionary
	#tag EndProperty


End Class
#tag EndClass

