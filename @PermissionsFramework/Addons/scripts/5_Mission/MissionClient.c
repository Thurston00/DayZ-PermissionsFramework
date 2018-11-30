modded class MissionGameplay
{
    protected ref PermissionsFramework m_PermissionsFramework;

    void MissionGameplay()
    {        
        m_PermissionsFramework = new PermissionsFramework();
    }

    void ~MissionGameplay()
    {
    }

    override void OnInit()
    {
        super.OnInit();
    }

    override void OnMissionStart()
    {
        if ( !GetGame().IsMultiplayer() )
        {
            GetPermissionsManager().PlayerJoined( NULL );
        }

        super.OnMissionStart();
    
        m_PermissionsFramework.OnStart();
        
        GetRPCManager().SendRPC( "PermissionsFramework", "UpdatePlayers", new Param, true );
    }

    override void OnMissionFinish()
    {
        m_PermissionsFramework.OnFinish();

        GetGame().GetUIManager().CloseMenu( MENU_INGAME );

        super.OnMissionFinish();
    }

    override void OnUpdate( float timeslice )
    {
        super.OnUpdate( timeslice );

        m_PermissionsFramework.OnUpdate( timeslice );
    }
}
