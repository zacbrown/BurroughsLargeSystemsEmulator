# Author: Shane Salta
# Date: 12.14.08
# Filename: string.p

export function strcpy;

##############################################################
# strcpy: copies a string to another string from start to end offsets
# * arg 1: string to be copied FROM
# * arg 2: string to be copied TO
# * arg 3: starting position for stringin
# * arg 4: ending position for stringout
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

