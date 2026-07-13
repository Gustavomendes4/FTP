
#include "clientHandlers.h"

#include "transfer.h"


const char *Token[] = {
    "get-file",
    "put-file",
    "delete-file",
    "list-file",
    "move-file",
    "copy-file",

    "create-dir",
    "delete-dir",
    "get-dir",
    "put-dir",
    "list-dir",
    "move-dir",
    "copy-dir",

    "ping",

    ""
};


void client_handleGetFile(ms_socket_t socket, Packet* pack, RequestDesc request){

    const char* filepath = request.arg1;
    const char* localpath = (request.arg2) ? request.arg2 : request.arg1;

    // Validate null filepath
    if( !filepath){
        fprintf(stderr, "File not informed to get-file\n");
        return;
    }

    /// ====    Send file requisition   ====
    if( sendPacketString(socket, MSG_GET_FILE, filepath) != 0){
        fprintf(stderr, "Error to send packet to server.\n");
        return;
    }


    int result = recvFile(socket, pack, localpath);



    if( result == 0){
        printf("File received successfully: %s\n", localpath);
    }

    else if( result == -1){
        fprintf(stderr, "Error to Write in: %s\n", localpath);

    }

    else{

        int type = (int)getPacketType(pack);

        switch(type){

            case MSG_ERROR_FILE_NOT_FOUND:
                fprintf(stderr, "Server: File not found[%d]\n", type);
                break;

            case MSG_ERROR_CANNOT_OPEN_FILE:
                fprintf(stderr, "Server: Error to open file[%d]\n", type);
                break;
            
            case MSG_ERROR_PERMISSION_DENIED:
                fprintf(stderr, "Server: Could not recive file, permisssion denied[%d]\n", type);
                break;

            default:
                fprintf(stderr, "Unexpected response in recv file: [%d]\n", type);
        }

    }
}

void client_handlePutFile(ms_socket_t socket, Packet* pack, RequestDesc request){

    const char* filepath = request.arg1;
    const char* remotepath = (request.arg2) ? request.arg2 : fc_getName(request.arg1);


    // =====  Validate args  =====
    if( !filepath || filepath[0] == '\0'){
        fprintf(stderr, "Invalid argument: file not informed");
        return;
    }

    if( !isValidPath(filepath) || !existsFile(filepath) ){
        fprintf(stderr, "File <%s> not exists\n", filepath);
        return;
    }

    /// ====    Send file requisition   ====
    if( sendPacketString(socket, MSG_PUT_FILE, remotepath) != 0){
        fprintf(stderr, "Error to send packet to server.\n");
        return;
    }

    int result = sendFile(socket, pack, filepath);

    if( result == 0){
        printf("File sent successfully: %s\n", remotepath);
    }

    else if( result == -1){
        fprintf(stderr, "Error to read in: %s\n", remotepath);
    }

    else{

        int type = (int)getPacketType(pack);

        switch(type){

            case MSG_ERROR_FILE_ALREADY_EXISTS: ;
                fprintf(stderr, "Sever: The file <%s> already exists\n", remotepath);

            case MSG_ERROR_CANNOT_OPEN_FILE:
                fprintf(stderr, "Server: Error to open file[%d]\n", type);
                break;
            
            case MSG_ERROR_PERMISSION_DENIED:
                fprintf(stderr, "Server: Could not recive file, permisssion denied[%d]\n", type);
                break;

            default:
                fprintf(stderr, "Unexpected response in recv file: [%d]\n", type);
        }

    }
}

void client_handleDeleteFile(ms_socket_t socket, Packet* pack, RequestDesc request){

    int ret;
    
    clearPacket(pack);

    const char* filepath = request.arg1;    
    char str[150];


    // =====  Validate args  =====
    if( !filepath || filepath[0] == '\0'){
        fprintf(stderr, "Invalid argument: file not informed");
        return;
    }

    // =====  Send Requisition  =====
    if( sendPacketString(socket, MSG_DELETE_FILE, filepath) != 0){
        fprintf(stderr, "Error to send packet to server.\n");
        return;
    }

    // =====  Recv response  =====
    
    ret = recvPacket(socket, pack);

    if( ret != 0){
        fprintf(stderr, "Error to recv response:[%d]\n", ret);
        return;
    }

    // === Validate ===

    int type = (int)getPacketType(pack);

    switch(type){

        case MSG_ERROR_FILE_NOT_FOUND:
            fprintf(stderr, "Server: File <%s> not found\n", filepath);
            break;

        case MSG_ERROR_PERMISSION_DENIED:

            packetReadStr(pack, str, 150);

            fprintf(stderr, "Server: Permission deined; %s\n");
            break;

        case MSG_OK:
            fprintf(stderr, "Server: File excluded successfullly.\n");
            break;

        case MSG_ERROR:
            fprintf(stderr, "Server: Unexpected error.\n");
            break;

        default: 
            fprintf(stderr, "Unexpected response type: [%d]\n", type);
    }
}

void client_handleMoveFile(ms_socket_t socket, Packet* pack, RequestDesc request){

    int ret;

    const char* path1 = request.arg1;
    const char* path2 = request.arg2;

    //  ====    Validate arg1    ====
    if( !path1 || path1[0] == '\0'){
        fprintf(stderr, "Invalid argument 1: file not informed");
        return;
    }

    if( !isValidPath(path1) ){
        fprintf(stderr, "Invalid argument 1: <%s> is not a valid path", path1);
        return;
    }

    //  ====    Validate arg2    ====
    if( !path2 || path2[0] == '\0'){
        fprintf(stderr, "Invalid argument 2: destiny not informed");
        return;
    }

    if( !isValidPath(path2) ){
        fprintf(stderr, "Invalid argument 2: <%s> is not a valid path", path2);
        return;
    }

    //  =============== Mount packet ===============
    clearPacket(pack);

    packetWriteString(pack, MSG_MOVE_FILE, path1);

    packetWriteString(pack, MSG_MOVE_FILE, path2);

    //  =============== Send packet ===============
    ret = sendPacket(socket, pack);

    if(ret != 0){
        fprintf(stderr, "Error to send request [%d]", ret);
        return;
    }


    //  ============ Recv response ============
    ret = recvPacket(socket, pack);

    if(ret != 0){
        fprintf(stderr, "Error to recv response [%d]", ret);
        return;
    }


    //  ============ Validate response ============
    int type = (int)getPacketType(pack);
    
    char msg[350] = "\0";

    switch(type){

        case MSG_BAD_REQUEST:
            packetReadStr(pack, msg, 350);
            fprintf(stderr, "Server: Bad request; %s", msg);
            break;

        case MSG_ERROR_FILE_NOT_FOUND:
            packetReadStr(pack, msg, 350);
            fprintf(stderr, "Server: File not found; %s", msg);
            break;

        case MSG_ERROR_PERMISSION_DENIED:
            packetReadStr(pack, msg, 350);
            fprintf(stderr, "Server: Permission denied; %s", msg);
            break;

        case MSG_OK:
            fprintf(stderr, "Server: File moved successfully");
            break;

        case MSG_ERROR:
            fprintf(stderr, "Server: Unexpected error to move file");
            break;

        case MSG_ERROR_FOLDER_NOT_EXISTS:
            packetReadStr(pack, msg, 350);
            fprintf(stderr, "Server: Folder not found; %s", msg);
            break;
            
        default: 
            fprintf(stderr, "Unexpected response type: [%d]\n", type);
    }
}

/**/void client_handleList(ms_socket_t socket, Packet* pack, RequestDesc request){

}

void client_handleCreateDir(ms_socket_t socket, Packet* pack, RequestDesc request){

    clearPacket(pack);

    int ret, response;
    char str[100];

    const char* remotepath = request.arg1 ? request.arg1 : NULL;

    // =====  Validate args  =====
    if( !remotepath || remotepath[0] == '\0'){
        fprintf(stderr, "Invalid argument: folder not informed");
        return;
    }

    if( !isValidPath(remotepath)){
        fprintf(stderr, "Folder name <%s> is not valid\n", remotepath);
        return;
    }

    /// ====    Send file requisition   ====
    if( sendPacketString(socket, MSG_CREATE_FOLDER, remotepath) != 0){
        fprintf(stderr, "Error to send packet to server.\n");
        return;
    }


    /// ====    Recv Response   ====
    ret = recvPacket(socket, pack);

    if(ret != 0){
        fprintf(stderr, "Error to recive response.s");
        return;
    }

    switch(response = (int)getPacketType(pack)){

        case MSG_ERROR_FOLDER_ALREADY_EXISTS:
            fprintf(stderr, "Server: Folder already exists\n");
            break;
        
        case MSG_OK:
            fprintf(stderr, "Server: Folder created successfully.\n");
            break;
            
        case MSG_ERROR:
            fprintf(stderr, "Server: Unexpected error\n");
            break;

        case MSG_BAD_REQUEST:
            fprintf(stderr, "Server: Bad request\n");
            break;

        case MSG_ERROR_PERMISSION_DENIED:
            packetReadStr(pack, str, 100);
            fprintf(stderr, "Server: Permission denied; %s");
            break;

        default:
            fprintf(stderr, "Unexpected response [%d]", (int)response);
    }
}

void client_handleDeleteDir(ms_socket_t socket, Packet* pack, RequestDesc request){

    clearPacket(pack);

    int ret, response;
    char str[100];

    const char* remotepath = request.arg1 ? request.arg1 : NULL;

    // =====  Validate args  =====
    if( !remotepath || remotepath[0] == '\0'){
        fprintf(stderr, "Invalid argument: folder not informed");
        return;
    }

    if( !isValidPath(remotepath)){
        fprintf(stderr, "Folder name <%s> is not valid\n", remotepath);
        return;
    }

    /// ====    Send file requisition   ====
    if( sendPacketString(socket, MSG_DELETE_FOLDER, remotepath) != 0){
        fprintf(stderr, "Error to send packet to server.\n");
        return;
    }


    /// ====    Recv Response   ====
    ret = recvPacket(socket, pack);

    if(ret != 0){
        fprintf(stderr, "Error to recive response\n");
        return;
    }

    switch(response = (int)getPacketType(pack)){

        case MSG_ERROR_FOLDER_NOT_EXISTS:
            fprintf(stderr, "Server: Folder not exists\n");
            break;
        
        case MSG_OK:
            fprintf(stderr, "Server: Folder deleted successfully.\n");
            break;
            
        case MSG_ERROR:
            fprintf(stderr, "Server: Unexpected error\n");
            break;

        case MSG_BAD_REQUEST:
            fprintf(stderr, "Server: Bad request\n");
            break;

        case MSG_ERROR_PERMISSION_DENIED:
            packetReadStr(pack, str, 100);
            fprintf(stderr, "Server: Permission denied; %s");
            break;

        default:
            fprintf(stderr, "Unexpected response [%d]", (int)response);
    }
}

/**/void client_handleGetDir(ms_socket_t socket, Packet* pack, RequestDesc request){

    printf("client handle: GetDir");
}

/**/void client_handlePutDir(ms_socket_t socket, Packet* pack, RequestDesc request){

    printf("client handle: PostDir");
}

void client_handleMoveDir(ms_socket_t socket, Packet* pack, RequestDesc request){

    int ret;

    const char* path1 = request.arg1;
    const char* path2 = request.arg2;

    //  ====    Validate arg1    ====
    if( !path1 || path1[0] == '\0'){
        fprintf(stderr, "Invalid argument 1: file not informed");
        return;
    }

    if( !isValidPath(path1) ){
        fprintf(stderr, "Invalid argument 1: <%s> is not a valid path", path1);
        return;
    }

    //  ====    Validate arg2    ====
    if( !path2 || path2[0] == '\0'){
        fprintf(stderr, "Invalid argument 2: destiny not informed");
        return;
    }

    if( !isValidPath(path2) ){
        fprintf(stderr, "Invalid argument 2: <%s> is not a valid path", path2);
        return;
    }

    //  =============== Mount packet ===============
    clearPacket(pack);

    packetWriteString(pack, MSG_MOVE_FOLDER, path1);

    packetWriteString(pack, MSG_MOVE_FOLDER, path2);

    //  =============== Send packet ===============
    ret = sendPacket(socket, pack);

    if(ret != 0){
        fprintf(stderr, "Error to send request [%d]", ret);
        return;
    }


    //  ============ Recv response ============
    ret = recvPacket(socket, pack);

    if(ret != 0){
        fprintf(stderr, "Error to recv response [%d]", ret);
        return;
    }


    //  ============ Validate response ============
    int type = (int)getPacketType(pack);
    
    char msg[350] = "\0";

    switch(type){

        case MSG_BAD_REQUEST:
            packetReadStr(pack, msg, 350);
            fprintf(stderr, "Server: Bad request; %s", msg);
            break;

        case MSG_ERROR_FOLDER_ALREADY_EXISTS:
            packetReadStr(pack, msg, 350);
            fprintf(stderr, "Server: Folder exists; %s", msg);
            break;

        case MSG_ERROR_PERMISSION_DENIED:
            packetReadStr(pack, msg, 350);
            fprintf(stderr, "Server: Permission denied; %s", msg);
            break;

        case MSG_OK:
            fprintf(stderr, "Server: Folder moved successfully");
            break;

        case MSG_ERROR:
            fprintf(stderr, "Server: Unexpected error to move folder");
            break;

        case MSG_ERROR_FOLDER_NOT_EXISTS:
            packetReadStr(pack, msg, 350);
            fprintf(stderr, "Server: Folder not found; %s", msg);
            break;
            
        default: 
            fprintf(stderr, "Unexpected response type: [%d]\n", type);
    }
}

void client_handlePingPong(ms_socket_t socket, Packet* pack, RequestDesc request){

    (request);

    clearPacket(pack);

    int ret;

    // =====  Send Ping Message  =====
    ret = sendTypedHeader(socket, MSG_PING_PONG);

    if( ret != 0){
        fprintf(stderr, "Error to send Ping TypedHeader -> [%d]\n", ret);
        return;
    }


    // =====  Recv Pong Message  =====
    
    ret = recvPacket(socket, pack);

    if( ret != 0){
        fprintf(stderr, "Error to recv Pong message -> [%d]\n", ret);
        return;
    }

    // === Validate ===

    int type = (int)getPacketType(pack);

    if( type != (int)MSG_PING_PONG){
        fprintf(stderr, "Unexpected response type: [%d]\n", type);
        return;
    }

    printf("Pong received successfully! \n");
}
