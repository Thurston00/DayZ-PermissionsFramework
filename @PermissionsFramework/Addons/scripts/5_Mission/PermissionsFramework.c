class PermissionsFramework
{
    protected bool m_bLoaded;

    void PermissionsFramework()
    {
        m_bLoaded = false;

        GetRPCManager().AddRPC( "PermissionsFramework", "UpdatePlayers", this, SingeplayerExecutionType.Server );
        GetRPCManager().AddRPC( "PermissionsFramework", "RemovePlayer", this, SingeplayerExecutionType.Client );
        GetRPCManager().AddRPC( "PermissionsFramework", "UpdatePlayer", this, SingeplayerExecutionType.Client );
    }

    void ~PermissionsFramework()
    {
        Print("PermissionsFramework::~PermissionsFramework");
    }
    
    void OnStart()
    {
    }

    void OnFinish()
    {
    }

    void OnLoaded()
    {
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

    void UpdatePlayers( CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target )
    {
        if ( type == CallType.Server )
        {
            if ( GetGame().IsMultiplayer() )
            {
                for ( int i = 0; i < GetPermissionsManager().GetPlayers().Count(); i++ )
                {
                    GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayer", new Param1< ref PlayerData >( SerializePlayer( GetPermissionsManager().GetPlayers().Get( i ) ) ), true, sender );
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
}