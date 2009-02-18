#test

main {
    local x, y; x = 0; y = 3;
    while (x < 10) do {
        if (x = 5) then break;
        x = (x + 1);
        while (y < x) do {
            if (x = y) then break;
            y = (y + 1)
        }
    };
    
    return
}
end
