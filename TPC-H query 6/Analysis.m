load("generated_data.csv");
dates = generated_data(:,2);
prices = generated_data(:,3);
discounts = generated_data(:,4);
quantities = generated_data(:,5);

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