class PermissionsFramework
{
    protected ref array< Man > m_ServerPlayers;

    protected bool m_bLoaded;

    void PermissionsFramework()
    {
        MakeDirectory( PERMISSION_FRAMEWORK_DIRECTORY );

        if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
        {
            m_ServerPlayers = new ref array< Man >;
        }

        m_bLoaded = false;

        GetRPCManager().AddRPC( "PermissionsFramework", "UpdatePlayers", this, SingeplayerExecutionType.Server );
        GetRPCManager().AddRPC( "PermissionsFramework", "RemovePlayer", this, SingeplayerExecutionType.Client );
        GetRPCManager().AddRPC( "PermissionsFramework", "UpdatePlayer", this, SingeplayerExecutionType.Client );
        GetRPCManager().AddRPC( "PermissionsFramework", "SetClientPlayer", this, SingeplayerExecutionType.Client );
    }

    void ~PermissionsFramework()
    {
        Print("PermissionsFramework::~PermissionsFramework");

        if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
        {
            GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Remove( this.ReloadPlayerList );

            delete m_ServerPlayers;
        }
    }
    
    void OnStart()
    {
        if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
        {
            GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( this.ReloadPlayerList, 1000, true );
        }
    }

    void OnFinish()
    {
    }

    void OnLoaded()
    {
        Print( "Requesting player list!" );
        GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayers", new Param, true );
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

    void ReloadPlayerList()
    {
        GetGame().GetPlayers( m_ServerPlayers );
        
        GetPlayerIndentities

        for ( int i = 0; i < AuthPlayers.Count(); i++ )
        {
            ref AuthPlayer auPlayer = AuthPlayers[i];
            
            if ( auPlayer.GetGUID() == player.GetId() )
            {
                auPlayer.Save();

                GetRPCManager().SendRPC( "PermissionsFramework", "RemovePlayer", new Param1< ref PlayerData >( SerializePlayer( auPlayer ) ), true );

                AuthPlayers.Remove( i );
                break;
            }
        }

        for ( int i = 0; i < m_ServerPlayers.Count(); i++ )
        {
            Man man = m_ServerPlayers[i];
            PlayerBase player = PlayerBase.Cast( man );

            ref AuthPlayer auPlayer = GetPermissionsManager().GetPlayerByIdentity( man.GetIdentity() );

            if ( player )
            {
                player.authenticatedPlayer = auPlayer;
            }

            auPlayer.PlayerObject = player;
            auPlayer.IdentityPlayer = man.GetIdentity();

            auPlayer.UpdatePlayerData();
        }

        m_ServerPlayers.Clear();
    }

    void UpdatePlayers( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
    {
        if ( type == CallType.Server )
        {
            if ( GetGame().IsMultiplayer() )
            {
                for ( int i = 0; i < GetPermissionsManager().GetPlayers().Count(); i++ )
                {
                    GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayer", new Param1< ref PlayerData >( SerializePlayer( GetPermissionsManager().GetPlayers()[i] ) ), true, sender );
                }
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

                GetPermissionsManager().AuthPlayers.RemoveItem( DeserializePlayer( data.param1 ) );
            }
        }
    }

    void UpdatePlayer( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
    {
        if ( type == CallType.Client )
        {
            if ( GetGame().IsMultiplayer() )
            {
                ref Param1< ref PlayerData > data;
                if ( !ctx.Read( data ) ) return;

                DeserializePlayer( data.param1 );
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
}