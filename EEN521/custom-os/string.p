# Author: Shane Salta
# Date: 12.14.08
# Filename: string.p

export function strcpy, strcut, strcat;

##############################################################
# strcpy: copies a string to another string from start to end offsets
# * arg 1: string to be copied FROM
# * arg 2: string to be copied TO
# * arg 3: starting char position for stringin
# * arg 4: ending char position for stringin
# * returns: 
#   * failure - 0
#   * success - 0
##############################################################
function strcpy(stringin, stringout, start_offset, end_offset ) {
    local i, j;
    i = start_offset;
	j = 0;
    while ( i <= end_offset) do
    {
        char j of stringout = char i of stringin;
		i = (i + 1);
		j = (j + 1);
	}
	return 0
}

##############################################################
# strcpy: cuts a string to another string from start to end offsets
# * arg 1: string to be cut FROM
# * arg 2: string to be pasted TO
# * arg 3: starting char position for stringin
# * arg 4: ending char position for stringin
# * arg 5: total size of stringin
# * returns: 
#   * failure - 0
#   * success - 0
##############################################################
function strcut(stringin, stringout, start_offset, end_offset, stringin_size ) {
	local i, j;
    i = start_offset;
	j = 0;
    while ( i <= end_offset) do
	{
		char j of stringout = char i of stringin;
		i = (i + 1);
		j = (j + 1);
	}
    i = start_offset;
    j = end_offset;
    while ( j < stringin_size && i < j ) do
    {
        char i of stringin = char j of stringin;
        char j of stringin = 0;
        i = (i + 1);
		j = (j + 1);
    }
    return 0
}

##############################################################
# strcat: adds a string to another string from start to end offsets
# * arg 1: string to be copied FROM
# * arg 2: string to be added TO
# * arg 3: starting char position for stringin
# * arg 4: ending char position for stringin
# * arg 5: char position to start append on stringout
# * returns:
#   * failure - 0
#   * success - 0
##############################################################
function strcat(stringin, stringout, start_offset, end_offset, 
        stringout_start) {
    local i, j;
    i = start_offset;
	j = stringout_start;
    while ( i <= end_offset) do
    {
        char j of stringout = char i of stringin;
		i = (i + 1);
		j = (j + 1);
	}
	return 0
}
