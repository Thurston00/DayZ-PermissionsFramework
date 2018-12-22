class AuthPlayer
{
    const string AUTH_DIRECTORY = PERMISSION_FRAMEWORK_DIRECTORY + "Permissions\\";
    const string FILE_TYPE = ".txt";

    ref Permission RootPermission;
    ref array< Role > Roles;

    PlayerBase PlayerObject;
    PlayerIdentity IdentityPlayer;

    ref PlayerData Data;

    protected ref PlayerFile m_PlayerFile;

    protected bool m_HasPermissions;
    protected bool m_HasPlayerData;

    void AuthPlayer( ref PlayerData data )
    {
        PlayerObject = NULL;

        Data = data;

        if ( Data == NULL )
        {
            Data = new ref PlayerData;
        }

        RootPermission = new ref Permission( Data.SSteam64ID );
        Roles = new ref array< Role >;

        m_PlayerFile = new ref PlayerFile;

        if ( GetGame().IsServer() )
        {
            MakeDirectory( AUTH_DIRECTORY );
            MakeDirectory( PERMISSION_FRAMEWORK_DIRECTORY + "Players\\" );
        }
    }

    void ~AuthPlayer()
    {
        delete RootPermission;
    }

    void SwapData( ref PlayerData newData )
    {
        Data = newData;
    }

    string GetGUID()
    {
        return Data.SGUID;
    }

    string GetSteam64ID()
    {
        return Data.SSteam64ID;
    }

    string GetName()
    {
        return Data.SName;
    }

    void UpdatePlayerData()
    {
        if ( IdentityPlayer == NULL ) return;

        Data.IPingMin = IdentityPlayer.GetPingMin();
        Data.IPingMax = IdentityPlayer.GetPingMax();
        Data.IPingAvg = IdentityPlayer.GetPingAvg();
        
        Data.SSteam64ID = IdentityPlayer.GetPlainId();
        Data.SGUID = IdentityPlayer.GetId();
        Data.SName = IdentityPlayer.GetName();

        if ( PlayerObject == NULL ) return;

        PlayerData.Load( Data, PlayerObject );
    }

    void CopyPermissions( ref Permission copy )
    {
        ref array< string > data = new ref array< string >;
        copy.Serialize( data );

        for ( int i = 0; i < data.Count(); i++ )
        {
            AddPermission( data[i] );
        }
    }

    void ClearPermissions()
    {
        delete RootPermission;

        RootPermission = new ref Permission( Data.SSteam64ID, NULL );

        m_HasPermissions = false;
    }

    void AddPermission( string permission, PermissionType type = PermissionType.INHERIT )
    {
        RootPermission.AddPermission( permission, type );

        m_HasPermissions = true;
    }

    bool HasPermission( string permission )
    {
        bool has = RootPermission.HasPermission( permission );

        if ( has )
            return true;

        for ( int j = 0; j < Roles.Count(); j++ )
        {
            if ( Roles[j].HasPermission( permission ) )
            {
                return true;
            }
        }

        return false;
    }

    void AddRole( Role role )
    {
        m_HasPlayerData = true;

        Roles.Insert( role );
    }

    ref array< string > Serialize()
    {        
        Data.APermissions.Clear();

        RootPermission.Serialize( Data.APermissions );

        return Data.APermissions;
    }

    void Deserialize()
    {
        for ( int i = 0; i < Data.APermissions.Count(); i++ )
        {
            AddPermission( Data.APermissions[i] );
        }
    }

    string FileReadyStripName( string name )
    {
        name.Replace( "\\", "" );
        name.Replace( "/", "" );
        name.Replace( "=", "" );
        name.Replace( "+", "" );

        return name;
    }

    void Save()
    {
        if ( m_HasPlayerData )
        {
            m_PlayerFile.Name = GetName();
            m_PlayerFile.GUID = GetGUID();
            m_PlayerFile.Steam64ID = GetSteam64ID();

            m_PlayerFile.Roles.Clear();

            for ( int j = 0; j < Roles.Count(); j++ )
            {
                m_PlayerFile.Roles.Insert( Roles[j].Name );
            }

            m_PlayerFile.Save();
        }

        if ( m_HasPermissions )
        {
            string filename = FileReadyStripName( Data.SSteam64ID );

            ref array< string > data = Serialize();
            Print( "Saving permissions and player data for " + filename );
            FileHandle file = OpenFile( AUTH_DIRECTORY + filename + FILE_TYPE, FileMode.WRITE );

            if ( file != 0 )
            {
                string line;

                for ( int i = 0; i < data.Count(); i++ )
                {
                    FPrintln( file, data[i] );
                }
                
                CloseFile(file);
            }
        }
    }

    bool Load()
    {
        string filename = FileReadyStripName( Data.SSteam64ID );
        Print( "Loading permissions for " + filename );
        FileHandle file = OpenFile( AUTH_DIRECTORY + filename + FILE_TYPE, FileMode.READ );
            
        ref array< string > data = new ref array< string >;

        m_HasPlayerData = m_PlayerFile.Load();

        if ( file != 0 )
        {
            string line;

            while ( FGets( file, line ) > 0 )
            {
                data.Insert( line );
            }

            CloseFile( file );

            for ( int i = 0; i < data.Count(); i++ )
            {
                AddPermission( data[i] );
            }

            m_HasPermissions = true;
        } else
        {
            m_HasPermissions = false;

            return false;
        }
        
        return true;
    }

    void DebugPrint()
    {
        Print( "Printing permissions for " + Data.SSteam64ID );

        RootPermission.DebugPrint( 0 );
    }

    // TODO: Figure out how to make it work properly?
    void Kick()
    {
    }

    // TODO: Maybe actually ban the player?
    void Ban()
    {
    }
}