int JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MAJOR = 0;
int JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MINOR = 6;
int JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_REVISION = 0;

class PermissionsFramework
{
    protected bool m_bLoaded;

    protected int m_Index;
    protected int m_UpdateMax;
    protected bool m_ClearNext;

    protected ref array< Man > m_Players;

    protected ref array< MinifiedPlayerData > m_PlayerListData;

    void PermissionsFramework()
    {
        MakeDirectory( PERMISSION_FRAMEWORK_DIRECTORY );

        m_Index = 0;
        m_UpdateMax = 5;

        m_bLoaded = false;
        m_Players = new ref array< Man >;
        m_PlayerListData = new ref array< MinifiedPlayerData >;

        GetRPCManager().AddRPC( "PermissionsFramework", "RemovePlayer", this, SingeplayerExecutionType.Client );
        GetRPCManager().AddRPC( "PermissionsFramework", "UpdatePlayers", this, SingeplayerExecutionType.Client );
        GetRPCManager().AddRPC( "PermissionsFramework", "UpdatePlayerData", this, SingeplayerExecutionType.Client )
        GetRPCManager().AddRPC( "PermissionsFramework", "UpdateRole", this, SingeplayerExecutionType.Client );
        GetRPCManager().AddRPC( "PermissionsFramework", "SetClientPlayer", this, SingeplayerExecutionType.Client );
        GetRPCManager().AddRPC( "PermissionsFramework", "CheckVersion", this, SingeplayerExecutionType.Server );

        GetPermissionsManager().RegisterPermission( "Admin.Player.Read" );
        GetPermissionsManager().RegisterPermission( "Admin.Roles.Update" );
    }

    void ~PermissionsFramework()
    {
        Print("PermissionsFramework::~PermissionsFramework");

        if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
        {
            GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Remove( this.ReloadPlayerList );
        }
    }
    
    void OnStart()
    {
        if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
        {
            GetPermissionsManager().LoadRoles();
        }
    }

    void OnFinish()
    {
    }

    void OnLoaded()
    {
        if ( GetGame().IsClient() && GetGame().IsMultiplayer() )
        {
            GetRPCManager().SendRPC( "PermissionsFramework", "CheckVersion", new Param3< int, int, int >( JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MAJOR, JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MINOR, JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_REVISION ), true );
        }

        if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
        {
            // GetGame().GetPlayers( m_Players );

            GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( this.ReloadPlayerList, 100, true );
        }
    }

    void Update( float timeslice )
    {
        if( !m_bLoaded && !GetDayZGame().IsLoading() )
        {
            m_bLoaded = true;
            OnLoaded();
        } else {
            OnUpdate( timeslice );
        }
    }

    void OnUpdate( float timeslice )
    {

    }

    ref AuthPlayer UpdateSpecificPlayer( PlayerBase player )
    {
        ref AuthPlayer auPlayer = GetPermissionsManager().GetPlayerByIdentity( player.GetIdentity() );

        if ( player )
        {
            player.authenticatedPlayer = auPlayer;
        }

        if ( auPlayer )
        {
            auPlayer.UpdatePlayerData();

            auPlayer.PlayerObject = player;
            auPlayer.IdentityPlayer = player.GetIdentity();

            return auPlayer;
        }

        return NULL;
    }

    void ReloadPlayerList()
    {
        int i = m_Index;

        if ( m_Players == NULL )
            return;

        if ( m_ClearNext )
        {
            m_PlayerListData.Clear();
            m_ClearNext = false;
        }

        for ( int j = 0; j < m_UpdateMax; j++ )
        {
            i = i + j;
            
            if ( i >= m_Players.Count() || i < 0 )
            {   
                if ( m_PlayerListData.Count() > 0 )
                {
                    GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayers", new Param1< ref array< MinifiedPlayerData > >( m_PlayerListData ), true, NULL );
                    m_ClearNext = true;
                }
                
                i = 0;

                GetGame().GetPlayers( m_Players );

                return;
            }

            ref AuthPlayer player = UpdateSpecificPlayer( PlayerBase.Cast( m_Players[i] ) );

            if ( player )
            {
                ref MinifiedPlayerData data = new ref MinifiedPlayerData;

                data.Name = player.GetName();
                data.SteamID = player.GetSteam64ID();
                data.GUID = player.GetGUID();
                data.Position = player.Data.VPosition;

                data.Obj = player.PlayerObject;

                m_PlayerListData.Insert( data );
            }
        }
    }

    void CheckVersion( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
    {
        Param3< int, int, int > data;
        if ( !ctx.Read( data ) ) return;

        if( type == CallType.Server )
        {
            if ( data.param1 != JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MAJOR )
            {
                Print( "" + sender.GetPlainId() + " is running a different major version of Permissions Framework." );
                return;
            }

            if ( data.param2 != JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_MINOR )
            {
                Print( "" + sender.GetPlainId() + " is running a different minor version of Permissions Framework." );
                return;
            }

            if ( data.param3 != JM_PERMISSIONS_FRAMEWORK_CURRENT_VERSION_REVISION )
            {
                Print( "" + sender.GetPlainId() + " is running a different revision of Permissions Framework." );       
                return;
            }
        }
    }

    void RemovePlayer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
    {
        if ( type == CallType.Client )
        {
            if ( GetGame().IsMultiplayer() )
            {
                ref Param1< ref PlayerData > data;
                if ( !ctx.Read( data ) ) return;
                
                ref AuthPlayer authPlayer = DeserializePlayer( data.param1 );

                RemoveSelectedPlayer( authPlayer );
                GetPermissionsManager().AuthPlayers.RemoveItem( authPlayer );
            }
        }
    }

    void UpdatePlayers( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
    {
        if ( type == CallType.Client )
        {
            if ( GetGame().IsMultiplayer() )
            {
                Print("--- Start of UpdatePlayers ---");

                ref Param1< ref array< MinifiedPlayerData > > data;
                if ( !ctx.Read( data ) ) return;

                m_PlayerListData.Copy( data.param1 );

                for ( int i = 0; i < m_PlayerListData.Count(); i++ )
                {
                    Print( m_PlayerListData[i] );

                    if ( m_PlayerListData[i] == NULL ) continue;

                    GetPermissionsManager().UpdateAuthPlayer( m_PlayerListData[i] );
                }
                Print("--- End of UpdatePlayers ---");
            }
        }
    }

    void UpdatePlayerData( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
    {
        if ( type == CallType.Server )
        {
            if ( !GetPermissionsManager().HasPermission( "Admin.Player.Read", sender ) )
                return;

            ref Param1< string > data;
            if ( !ctx.Read( data ) ) return;

            if ( GetGame().IsMultiplayer() )
            {
                ref AuthPlayer player = GetPermissionsManager().GetPlayerByGUID( data.param1 );
                if ( !player ) return;

                GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayerData", new Param1< ref PlayerData >( SerializePlayer( player ) ), true, sender, player.PlayerObject );
            }
        }

        if ( type == CallType.Client )
        {
            if ( GetGame().IsMultiplayer() )
            {
                ref Param1< ref PlayerData > cdata;
                if ( !ctx.Read( cdata ) ) return;

                DeserializePlayer( cdata.param1 ).PlayerObject = target;
            }
        }
    }

    void SetClientPlayer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
    {
        if ( type == CallType.Client )
        {
            if ( GetGame().IsMultiplayer() )
            {
                ref Param1< ref PlayerData > data;
                if ( !ctx.Read( data ) ) return;

                ClientAuthPlayer = DeserializePlayer( data.param1 );
            }
        }
    }

    void UpdateRole( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
    {
        ref Param2< string, ref array< string > > data;
        if ( !ctx.Read( data ) ) return;

        ref array< string > arr = new ref array< string >;
        arr.Copy( data.param2 );

        ref Role role = NULL;

        if ( type == CallType.Server )
        {
            if ( !GetPermissionsManager().HasPermission( "Admin.Roles.Update", sender ) )
                return;

            GetPermissionsManager().RolesMap.Find( data.param1, role );

            if ( role )
            {
                role.ClearPermissions();

                role.SerializedData = arr;

                role.Deserialize();
            } else 
            {
                role = GetPermissionsManager().LoadRole( data.param1, arr );
            }

            role.Serialize();
                
            GetRPCManager().SendRPC( "PermissionsFramework", "UpdateRole", new Param2< string, ref array< string > >( role.Name, role.SerializedData ), true );
        }

        if ( type == CallType.Client )
        {
            GetPermissionsManager().RolesMap.Find( data.param1, role );

            if ( role )
            {
                role.ClearPermissions();

                role.SerializedData = arr;

                role.Deserialize();
            } else 
            {
                GetPermissionsManager().LoadRole( data.param1, arr );
            }
        }
    }
}