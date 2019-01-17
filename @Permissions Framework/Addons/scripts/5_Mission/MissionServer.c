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

// TODO: Fully implement this as a better way for permissions
/*
    override void OnEvent( EventType eventTypeId, Param params ) 
	{
		super.OnEvent( eventTypeId, params );
		PlayerIdentity identity;

		switch(eventTypeId)
		{
			case ChatMessageEventTypeID:
                ChatMessageEventParams chat_params = ChatMessageEventParams.Cast( params );

                if ( chat_params.param1 == 0 && chat_params.param2 != "" )
                {
                    if ( chat_params.param3 == "/givefull" )
                    {

                    }
                }
                break;
		}
	}
*/

	override void OnPreloadEvent(PlayerIdentity identity, out bool useDB, out vector pos, out float yaw, out int queueTime)
	{
        super.OnPreloadEvent( identity, useDB, pos, yaw, queueTime );

        //GetPermissionsManager().GetPlayerByIdentity( identity );
    }

    override void InvokeOnConnect( PlayerBase player, PlayerIdentity identity)
	{
        super.InvokeOnConnect( player, identity );

        //GetPermissionsManager().GetPlayerByIdentity( identity );

        SetClientPermissionsOnConnection( identity, player );
    } 

	override void OnClientReconnectEvent( PlayerIdentity identity, PlayerBase player )
	{
        super.OnClientReconnectEvent( identity, player );
        
        //GetPermissionsManager().GetPlayerByIdentity( identity );
        
        SetClientPermissionsOnConnection( identity, player );
    }

	override void OnClientReadyEvent(PlayerIdentity identity, PlayerBase player)
	{
        super.OnClientReadyEvent( identity, player );
        
        //GetPermissionsManager().GetPlayerByIdentity( identity );
        
        SetClientPermissionsOnConnection( identity, player );
	}	

/*
	override void OnClientRespawnEvent(PlayerIdentity identity, PlayerBase player)
	{
        super.OnClientRespawnEvent( identity, player );
        
        //GetPermissionsManager().GetPlayerByIdentity( identity );
        
        //SetClientPermissionsOnConnection( identity, player );
    }

    override void HandleBody(PlayerBase player)
	{
        //GetPermissionsManager().PlayerLeft( player.GetIdentity() );

        super.HandleBody( player );
	}
*/

/*
    override void PlayerDisconnected( PlayerBase player, PlayerIdentity identity, string uid )
	{
        //GetPermissionsManager().PlayerLeft( identity );

        super.PlayerDisconnected( player, identity, uid );
*/
/*
        for ( int i = 0; i < GetPermissionsManager().AuthPlayers.Count(); i++ )
        {
            ref AuthPlayer ap = GetPermissionsManager().AuthPlayers[i];

            for ( int j = 0; j < identities.Count(); j++ )
            {
                if ( ap.GetGUID() == identities[j].GetId() )
                {
                    ap.Save();

                    GetRPCManager().SendRPC( "PermissionsFramework", "RemovePlayer", new Param1< ref PlayerData >( SerializePlayer( ap ) ), true );

                    GetPermissionsManager().AuthPlayers.Remove( i );

                    i = i - 1;
                }
            }
        }
*/
//    } 

    void SetClientPermissionsOnConnection( PlayerIdentity identity, PlayerBase player )
    {
        for ( int i = 0; i < GetPermissionsManager().Roles.Count(); i++ )
        {
            ref Role role = GetPermissionsManager().Roles[i];
            role.Serialize();
            GetRPCManager().SendRPC( "PermissionsFramework", "UpdateRole", new Param2< string, ref array< string > >( role.Name, role.SerializedData ), true, identity );
        }

        ref AuthPlayer auPlayer = GetPermissionsManager().GetPlayerByIdentity( identity );

        auPlayer.PlayerObject = player;

        GetRPCManager().SendRPC( "PermissionsFramework", "SetClientPlayer", new Param1< ref PlayerData >( SerializePlayer( auPlayer ) ), true, identity );
    }

}
