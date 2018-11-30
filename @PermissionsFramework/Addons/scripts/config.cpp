class CfgPatches
{
    class Permissions_Framework_Scripts
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]=
        {
            "RPC_Scripts", "DZ_Data"
        };
    };
};

class CfgMods
{
    class Permissions_Framework
    {
        dir = "PermissionsFramework";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Permissions Framework";
        credits = "Jacob_Mango";
        author = "Jacob_Mango";
        authorID = "0"; 
        version = "1.0"; 
        extra = 0;
        type = "mod";
        
        dependencies[] = {"Game", "World", "Mission"};
        
        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"PermissionsFramework/Scripts/3_Game"};
            };

            class worldScriptModule
            {
                value = "";
                files[] = {"PermissionsFramework/Scripts/4_World"};
            };

            class missionScriptModule
            {
                value = "";
                files[] = {"PermissionsFramework/Scripts/5_Mission"};
            };
        }
    };
};