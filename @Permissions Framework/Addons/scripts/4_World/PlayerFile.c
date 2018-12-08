class PlayerFile
{
    string Name;
    string GUID;
    string Steam64ID;

    ref array< string > Roles;

    void PlayerFile()
    {
        Roles = new ref array< string >;
    }

    static ref PlayerFile Load()
    {
        ref PlayerFile settings = new PlayerFile();

        if ( FileExist( PERMISSION_FRAMEWORK_DIRECTORY + "Players\\" + settings.Steam64ID + ".json" ) )
        {
            JsonFileLoader<PlayerFile>.JsonLoadFile( PERMISSION_FRAMEWORK_DIRECTORY + "Players\\" + settings.Steam64ID + ".json", settings );
        }

        return settings;
    }

    void Save()
    {
        JsonFileLoader<PlayerFile>.JsonSaveFile( PERMISSION_FRAMEWORK_DIRECTORY + "Players\\" + Steam64ID + ".json", this );
    }
}