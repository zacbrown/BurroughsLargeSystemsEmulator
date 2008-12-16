# Author: Zac Brown
# Date: 12.14.08
# Filename: flags.p

export function setError, setInterrupt, getInterrupt, getError;

############################################
# setInterrupt: set the value of the IP flag
# * returns: 
#   * value of error flag
############################################
function setInterrupt(value) {
    <| LOAD R1, <value>;
    <| SETFL R1, $IP;
    
    return
}

############################################
# getInterrupt: get the value of the IP flag
# * returns: 
#   * value of error flag
############################################
function getInterrupt() {
    <| GETFL R0, $IP;
    
    return
}

############################################
# setError: set the value of the IP flag
# * returns: 
#   * value of error flag
############################################
function setError(value) {
    <| LOAD R1, <value>;
    <| SETFL R1, $IP;
    
    return
}

############################################
# getError: get the value of the ERR flag
# * returns: 
#   * value of error flag
############################################
function getError() {
    <| GETFL R0, $ERR;
    return
}

end
