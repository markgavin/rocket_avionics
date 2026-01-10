#tag Class
Protected Class FlightDatabase
	#tag Method, Flags = &h0
		Sub Close()
		  // Close database connection
		  If pDatabase <> Nil Then
		    pDatabase.Close
		  End If
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub Constructor()
		  // Initialize database connection
		  pDatabase = New SQLiteDatabase

		  // Get path to documents folder
		  Var theDocsFolder As FolderItem = SpecialFolder.Documents
		  pDatabasePath = theDocsFolder.Child(kDatabaseName)

		  pDatabase.DatabaseFile = pDatabasePath
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h21
		Private Sub CreateTables()
		  // Create database schema

		  // Flights table
		  Var theFlightsSQL As String = _
		    "CREATE TABLE IF NOT EXISTS flights (" + _
		    "flight_id TEXT PRIMARY KEY, " + _
		    "timestamp TEXT NOT NULL, " + _
		    "pilot_name TEXT, " + _
		    "rocket_name TEXT, " + _
		    "motor_designation TEXT, " + _
		    "location TEXT, " + _
		    "notes TEXT, " + _
		    "max_altitude_m REAL, " + _
		    "max_velocity_mps REAL, " + _
		    "apogee_time_ms INTEGER, " + _
		    "flight_time_ms INTEGER, " + _
		    "created_at TEXT DEFAULT CURRENT_TIMESTAMP)"

		  pDatabase.ExecuteSQL(theFlightsSQL)

		  // Samples table
		  Var theSamplesSQL As String = _
		    "CREATE TABLE IF NOT EXISTS samples (" + _
		    "id INTEGER PRIMARY KEY AUTOINCREMENT, " + _
		    "flight_id TEXT NOT NULL, " + _
		    "time_ms INTEGER NOT NULL, " + _
		    "altitude_m REAL NOT NULL, " + _
		    "velocity_mps REAL, " + _
		    "pressure_pa REAL, " + _
		    "temperature_c REAL, " + _
		    "state TEXT, " + _
		    "FOREIGN KEY (flight_id) REFERENCES flights(flight_id))"

		  pDatabase.ExecuteSQL(theSamplesSQL)

		  // Index on samples
		  Var theIndexSQL As String = _
		    "CREATE INDEX IF NOT EXISTS idx_samples_flight_id ON samples(flight_id)"

		  pDatabase.ExecuteSQL(theIndexSQL)

		  // Settings table
		  Var theSettingsSQL As String = _
		    "CREATE TABLE IF NOT EXISTS settings (" + _
		    "key TEXT PRIMARY KEY, " + _
		    "value TEXT)"

		  pDatabase.ExecuteSQL(theSettingsSQL)
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function DeleteFlight(inFlightId As String) As Boolean
		  // Delete a flight from database
		  Try
		    pDatabase.BeginTransaction

		    // Delete samples
		    Var theSamplesSQL As String = "DELETE FROM samples WHERE flight_id = ?"
		    Var theSamplesStmt As PreparedSQLStatement = pDatabase.Prepare(theSamplesSQL)
		    theSamplesStmt.BindType(0, SQLitePreparedStatement.SQLITE_TEXT)
		    theSamplesStmt.Bind(0, inFlightId)
		    theSamplesStmt.ExecuteSQL

		    // Delete flight
		    Var theFlightSQL As String = "DELETE FROM flights WHERE flight_id = ?"
		    Var theFlightStmt As PreparedSQLStatement = pDatabase.Prepare(theFlightSQL)
		    theFlightStmt.BindType(0, SQLitePreparedStatement.SQLITE_TEXT)
		    theFlightStmt.Bind(0, inFlightId)
		    theFlightStmt.ExecuteSQL

		    pDatabase.CommitTransaction

		    Return True

		  Catch theError As DatabaseException
		    pDatabase.RollbackTransaction
		    Return False
		  End Try
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetLastError() As String
		  Return pLastError
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetSetting(inKey As String, inDefault As String = "") As String
		  // Get a setting value
		  Try
		    Var theSQL As String = "SELECT value FROM settings WHERE key = ?"
		    Var theStmt As PreparedSQLStatement = pDatabase.Prepare(theSQL)
		    theStmt.BindType(0, SQLitePreparedStatement.SQLITE_TEXT)
		    theStmt.Bind(0, inKey)

		    Var theResult As RowSet = theStmt.SelectSQL

		    If theResult <> Nil And theResult.RowCount > 0 Then
		      Return theResult.Column("value").StringValue
		    End If

		  Catch theError As DatabaseException
		    // Return default on error
		  End Try

		  Return inDefault
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function GetFlightList() As Dictionary()
		  // Get list of all flights
		  Var theFlights() As Dictionary

		  Try
		    Var theSQL As String = _
		      "SELECT flight_id, timestamp, rocket_name, motor_designation, max_altitude_m, " + _
		      "max_velocity_mps, flight_time_ms FROM flights ORDER BY timestamp DESC"

		    Var theResult As RowSet = pDatabase.SelectSQL(theSQL)

		    While Not theResult.AfterLastRow
		      Var theFlight As New Dictionary

		      theFlight.Value("flight_id") = theResult.Column("flight_id").StringValue
		      theFlight.Value("timestamp") = theResult.Column("timestamp").StringValue
		      theFlight.Value("rocket") = theResult.Column("rocket_name").StringValue
		      theFlight.Value("motor") = theResult.Column("motor_designation").StringValue
		      theFlight.Value("max_altitude") = theResult.Column("max_altitude_m").DoubleValue
		      theFlight.Value("max_velocity") = theResult.Column("max_velocity_mps").DoubleValue
		      theFlight.Value("flight_time") = theResult.Column("flight_time_ms").IntegerValue

		      theFlights.Add(theFlight)
		      theResult.MoveToNextRow
		    Wend

		    theResult.Close

		  Catch theError As DatabaseException
		    // Return empty array on error
		  End Try

		  Return theFlights
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function LoadFlight(inFlightId As String) As FlightData
		  // Load flight data from database
		  Try
		    Var theFlightSQL As String = "SELECT * FROM flights WHERE flight_id = ?"

		    Var theFlightStmt As PreparedSQLStatement = pDatabase.Prepare(theFlightSQL)
		    theFlightStmt.BindType(0, SQLitePreparedStatement.SQLITE_TEXT)
		    theFlightStmt.Bind(0, inFlightId)

		    Var theFlightResult As RowSet = theFlightStmt.SelectSQL

		    If theFlightResult = Nil Or theFlightResult.RowCount = 0 Then
		      Return Nil
		    End If

		    Var theFlightData As New FlightData

		    theFlightData.pFlightId = theFlightResult.Column("flight_id").StringValue
		    theFlightData.pPilotName = theFlightResult.Column("pilot_name").StringValue
		    theFlightData.pRocketName = theFlightResult.Column("rocket_name").StringValue
		    theFlightData.pMotorDesignation = theFlightResult.Column("motor_designation").StringValue
		    theFlightData.pLocation = theFlightResult.Column("location").StringValue
		    theFlightData.pNotes = theFlightResult.Column("notes").StringValue
		    theFlightData.pMaxAltitudeM = theFlightResult.Column("max_altitude_m").DoubleValue
		    theFlightData.pMaxVelocityMps = theFlightResult.Column("max_velocity_mps").DoubleValue
		    theFlightData.pApogeeTimeMs = theFlightResult.Column("apogee_time_ms").IntegerValue
		    theFlightData.pFlightTimeMs = theFlightResult.Column("flight_time_ms").IntegerValue

		    theFlightResult.Close

		    // Load samples
		    Var theSamplesSQL As String = _
		      "SELECT time_ms, altitude_m, velocity_mps, pressure_pa, temperature_c, state " + _
		      "FROM samples WHERE flight_id = ? ORDER BY time_ms"

		    Var theSamplesStmt As PreparedSQLStatement = pDatabase.Prepare(theSamplesSQL)
		    theSamplesStmt.BindType(0, SQLitePreparedStatement.SQLITE_TEXT)
		    theSamplesStmt.Bind(0, inFlightId)

		    Var theSamplesResult As RowSet = theSamplesStmt.SelectSQL

		    While Not theSamplesResult.AfterLastRow
		      Var theSample As New TelemetrySample
		      theSample.pTimeMs = theSamplesResult.Column("time_ms").IntegerValue
		      theSample.pAltitudeM = theSamplesResult.Column("altitude_m").DoubleValue
		      theSample.pVelocityMps = theSamplesResult.Column("velocity_mps").DoubleValue
		      theSample.pPressurePa = theSamplesResult.Column("pressure_pa").DoubleValue
		      theSample.pTemperatureC = theSamplesResult.Column("temperature_c").DoubleValue
		      theSample.pState = theSamplesResult.Column("state").StringValue
		      theFlightData.pSamples.Add(theSample)
		      theSamplesResult.MoveToNextRow
		    Wend

		    theSamplesResult.Close

		    Return theFlightData

		  Catch theError As DatabaseException
		    Return Nil
		  End Try
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function Open() As Boolean
		  // Open or create the database
		  Try
		    If pDatabasePath.Exists Then
		      pDatabase.Connect
		    Else
		      pDatabase.CreateDatabase
		    End If

		    // Create tables if needed
		    CreateTables()

		    Return True

		  Catch theError As DatabaseException
		    pLastError = theError.Message
		    Return False
		  End Try
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Function SaveFlight(inFlightData As FlightData) As Boolean
		  // Save flight data to database
		  Try
		    pDatabase.BeginTransaction

		    // Insert flight record
		    Var theFlightSQL As String = _
		      "INSERT OR REPLACE INTO flights " + _
		      "(flight_id, timestamp, pilot_name, rocket_name, motor_designation, " + _
		      "location, notes, max_altitude_m, max_velocity_mps, " + _
		      "apogee_time_ms, flight_time_ms) " + _
		      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"

		    Var theFlightStmt As PreparedSQLStatement = pDatabase.Prepare(theFlightSQL)

		    theFlightStmt.BindType(0, SQLitePreparedStatement.SQLITE_TEXT)
		    theFlightStmt.BindType(1, SQLitePreparedStatement.SQLITE_TEXT)
		    theFlightStmt.BindType(2, SQLitePreparedStatement.SQLITE_TEXT)
		    theFlightStmt.BindType(3, SQLitePreparedStatement.SQLITE_TEXT)
		    theFlightStmt.BindType(4, SQLitePreparedStatement.SQLITE_TEXT)
		    theFlightStmt.BindType(5, SQLitePreparedStatement.SQLITE_TEXT)
		    theFlightStmt.BindType(6, SQLitePreparedStatement.SQLITE_TEXT)
		    theFlightStmt.BindType(7, SQLitePreparedStatement.SQLITE_DOUBLE)
		    theFlightStmt.BindType(8, SQLitePreparedStatement.SQLITE_DOUBLE)
		    theFlightStmt.BindType(9, SQLitePreparedStatement.SQLITE_INTEGER)
		    theFlightStmt.BindType(10, SQLitePreparedStatement.SQLITE_INTEGER)

		    theFlightStmt.Bind(0, inFlightData.pFlightId)
		    theFlightStmt.Bind(1, inFlightData.pTimestamp.ToString)
		    theFlightStmt.Bind(2, inFlightData.pPilotName)
		    theFlightStmt.Bind(3, inFlightData.pRocketName)
		    theFlightStmt.Bind(4, inFlightData.pMotorDesignation)
		    theFlightStmt.Bind(5, inFlightData.pLocation)
		    theFlightStmt.Bind(6, inFlightData.pNotes)
		    theFlightStmt.Bind(7, inFlightData.pMaxAltitudeM)
		    theFlightStmt.Bind(8, inFlightData.pMaxVelocityMps)
		    theFlightStmt.Bind(9, inFlightData.pApogeeTimeMs)
		    theFlightStmt.Bind(10, inFlightData.pFlightTimeMs)

		    theFlightStmt.ExecuteSQL

		    // Delete existing samples for this flight
		    Var theDeleteSQL As String = "DELETE FROM samples WHERE flight_id = ?"
		    Var theDeleteStmt As PreparedSQLStatement = pDatabase.Prepare(theDeleteSQL)
		    theDeleteStmt.BindType(0, SQLitePreparedStatement.SQLITE_TEXT)
		    theDeleteStmt.Bind(0, inFlightData.pFlightId)
		    theDeleteStmt.ExecuteSQL

		    // Insert samples
		    Var theSampleSQL As String = _
		      "INSERT INTO samples (flight_id, time_ms, altitude_m, velocity_mps, " + _
		      "pressure_pa, temperature_c, state) VALUES (?, ?, ?, ?, ?, ?, ?)"

		    For Each theSample As TelemetrySample In inFlightData.pSamples
		      Var theSampleStmt As PreparedSQLStatement = pDatabase.Prepare(theSampleSQL)

		      theSampleStmt.BindType(0, SQLitePreparedStatement.SQLITE_TEXT)
		      theSampleStmt.BindType(1, SQLitePreparedStatement.SQLITE_INTEGER)
		      theSampleStmt.BindType(2, SQLitePreparedStatement.SQLITE_DOUBLE)
		      theSampleStmt.BindType(3, SQLitePreparedStatement.SQLITE_DOUBLE)
		      theSampleStmt.BindType(4, SQLitePreparedStatement.SQLITE_DOUBLE)
		      theSampleStmt.BindType(5, SQLitePreparedStatement.SQLITE_DOUBLE)
		      theSampleStmt.BindType(6, SQLitePreparedStatement.SQLITE_TEXT)

		      theSampleStmt.Bind(0, inFlightData.pFlightId)
		      theSampleStmt.Bind(1, theSample.pTimeMs)
		      theSampleStmt.Bind(2, theSample.pAltitudeM)
		      theSampleStmt.Bind(3, theSample.pVelocityMps)
		      theSampleStmt.Bind(4, theSample.pPressurePa)
		      theSampleStmt.Bind(5, theSample.pTemperatureC)
		      theSampleStmt.Bind(6, theSample.pState)

		      theSampleStmt.ExecuteSQL
		    Next

		    pDatabase.CommitTransaction

		    Return True

		  Catch theError As DatabaseException
		    pDatabase.RollbackTransaction
		    pLastError = theError.Message
		    Return False
		  End Try
		End Function
	#tag EndMethod

	#tag Method, Flags = &h0
		Sub SetSetting(inKey As String, inValue As String)
		  // Save a setting value
		  Try
		    Var theSQL As String = _
		      "INSERT OR REPLACE INTO settings (key, value) VALUES (?, ?)"
		    Var theStmt As PreparedSQLStatement = pDatabase.Prepare(theSQL)
		    theStmt.BindType(0, SQLitePreparedStatement.SQLITE_TEXT)
		    theStmt.BindType(1, SQLitePreparedStatement.SQLITE_TEXT)
		    theStmt.Bind(0, inKey)
		    theStmt.Bind(1, inValue)
		    theStmt.ExecuteSQL

		  Catch theError As DatabaseException
		    // Ignore errors
		  End Try
		End Sub
	#tag EndMethod

	#tag Method, Flags = &h0
		Function ExportAllFlights() As String
		  // Export all flights as JSON
		  Var theExport As New JSONItem

		  Var theFlightsArray As New JSONItem

		  Var theFlightList() As Dictionary = GetFlightList()

		  For Each theFlightSummary As Dictionary In theFlightList
		    Var theFlightId As String = theFlightSummary.Value("flight_id")
		    Var theFlightData As FlightData = LoadFlight(theFlightId)

		    If theFlightData IsA FlightData Then
		      Var theFlightJson As New JSONItem(theFlightData.ToJSON)
		      theFlightsArray.Add(theFlightJson)
		    End If
		  Next

		  theExport.Value("version") = kSchemaVersion
		  theExport.Value("export_date") = DateTime.Now.ToString
		  theExport.Value("flights") = theFlightsArray

		  Return theExport.ToString
		End Function
	#tag EndMethod


	#tag Property, Flags = &h21
		Private pDatabase As SQLiteDatabase
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pDatabasePath As FolderItem
	#tag EndProperty

	#tag Property, Flags = &h21
		Private pLastError As String = ""
	#tag EndProperty


	#tag Constant, Name = kDatabaseName, Type = String, Dynamic = False, Default = \"RocketAvionics.sqlite", Scope = Private
	#tag EndConstant

	#tag Constant, Name = kSchemaVersion, Type = Double, Dynamic = False, Default = \"1", Scope = Private
	#tag EndConstant


End Class
#tag EndClass
