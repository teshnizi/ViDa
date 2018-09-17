#load("cmake-build-debug/edited_lineitem.tbl");
dates = edited_lineitem(:,5);
prices = edited_lineitem(:,3);
discounts = edited_lineitem(:,4);
quantities = edited_lineitem(:,2);

hist(prices);
title("prices");
pause(5)

hist(dates);
title("dates");
pause(5)

hist(quantities);
title("quantities");
pause(5)

hist(discounts);
title("discounts");
pause(5)