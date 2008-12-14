# Author: Zac Brown
# Date: 12.08.08
# Filename: error.p

export function isErrorSet;

############################################
# isErrorSet: get the value of the ERR flag
# * returns: 
#   * value of error flag
############################################
function isErrorSet() {
    <| GETFL R0, $ERR;
    return
}

end