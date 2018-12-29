class PlayerFile
{
    ref array< string > Names;
    string GUID;
    string Steam64ID;

    ref array< string > Roles;

    void PlayerFile()
    {
        Roles = new ref array< string >;
    }

    bool Load( PlayerIdentity identity )
    {
        if ( FileExist( PERMISSION_FRAMEWORK_DIRECTORY + "Players\\" + identity.GetPlainId() + ".json" ) )
        {
            JsonFileLoader<PlayerFile>.JsonLoadFile( PERMISSION_FRAMEWORK_DIRECTORY + "Players\\" + identity.GetPlainId() + ".json", this );

            bool requiresSaving = false;

            if ( Steam64ID != identity.GetPlainId() )
            {
                Steam64ID = identity.GetPlainId();
                requiresSaving = true;
            }

            if ( GUID != identity.GetId() )
            {
                GUID = identity.GetId();
                requiresSaving = true;
            }

            if ( Names.Find( identity.GetName() ) < 0 )
            {   
                Names.Insert( identity.GetName() );
                requiresSaving = true;
            }

            if ( requiresSaving )
            {
                Save();
            }
            
            return true;
        }

        Roles.Insert( "everyone" );

        return false;
    }

    void Save()
    {
        JsonFileLoader<PlayerFile>.JsonSaveFile( PERMISSION_FRAMEWORK_DIRECTORY + "Players\\" + Steam64ID + ".json", this );
    }
}