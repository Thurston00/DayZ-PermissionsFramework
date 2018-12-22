modded class MissionServer
{
    protected ref PermissionsFramework m_PermissionsFramework;

    void MissionServer()
    {        
        m_PermissionsFramework = new ref PermissionsFramework;
    }

    void ~MissionServer()
    {
        delete m_PermissionsFramework;
    }

    override void OnInit()
    {
        super.OnInit();
    }

    override void OnMissionStart()
    {
        super.OnMissionStart();

        m_PermissionsFramework.OnStart();
    }

    override void OnMissionFinish()
    {
        m_PermissionsFramework.OnFinish();

        super.OnMissionFinish();
    }

    override void OnUpdate( float timeslice )
    {
        super.OnUpdate( timeslice );

        m_PermissionsFramework.Update( timeslice );
    }

	override void OnPreloadEvent(PlayerIdentity identity, out bool useDB, out vector pos, out float yaw, out int queueTime)
	{
        super.OnPreloadEvent( identity, useDB, pos, yaw, queueTime );

        GetPermissionsManager().GetPlayerByIdentity( identity );
    }

    override void InvokeOnConnect( PlayerBase player, PlayerIdentity identity)
	{
        super.InvokeOnConnect( player, identity );
        
        GetRPCManager().SendRPC( "PermissionsFramework", "SetClientPlayer", new Param1< ref PlayerData >( SerializePlayer( GetPermissionsManager().GetPlayerByIdentity( identity ) ) ), true, identity );

        GetGame().SelectPlayer( identity, player );
    } 

    override void PlayerDisconnected( PlayerBase player, PlayerIdentity identity, string uid )
	{
        GetPermissionsManager().PlayerLeft( identity );

        super.PlayerDisconnected( player, identity, uid );
    } 

}
