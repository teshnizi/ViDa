
#include "database_preparation.h"

using namespace std;

	double sum_price = 0;

int main(){


	int lineitem_ia[] = {ldiscount_id, lprice_id, lshipdate_id, lquantity_id};
	int lineitem_sa[] = {lshipinstruct_id, lshipmode_id};
    read_lineitems_from_file("edited_lineitem.tbl", lineitem_ia, 4, lineitem_sa, 2);
    read_histogram_defaults_from_file("histograms.txt");
	//SCANNING ATTRIBUTES(TAKING A COPY FROM DEFAULTS)

	{
		Histogram ldiscount_hist;
		ldiscount_hist.minimum = default_histogram[lineitem][ldiscount_id].minimum;
		ldiscount_hist.maximum = default_histogram[lineitem][ldiscount_id].maximum;
		ldiscount_hist.bucket_count = default_histogram[lineitem][ldiscount_id].bucket_count;
		ldiscount_hist.bucket_size = default_histogram[lineitem][ldiscount_id].bucket_size;
		for (int i = 0; i < ldiscount_hist.bucket_count; ++i){
		    ldiscount_hist.bucket[i] = default_histogram[lineitem][ldiscount_id].bucket[i];
		}
		for (int lineitem_it = 0; lineitem_it < table_size[lineitem]; lineitem_it++)
		{
			int lpartkey = int_table[lineitem][lpartkey_id][lineitem_it];

			int lprice = int_table[lineitem][lprice_id][lineitem_it];

			int lquantity = int_table[lineitem][lquantity_id][lineitem_it];

			int lshipdate = int_table[lineitem][lshipdate_id][lineitem_it];

			string lshipinstruct = string_table[lineitem][lshipinstruct_id][lineitem_it];

			string lshipmode = string_table[lineitem][lshipmode_id][lineitem_it];

			double lineitem_table_size = 1;
			//APPLYING THE CONDITIONS OF EVERY SUBCONDITION INSIDE SELECT NODE

			{
				//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION

				int Select_lineitem_table_size = lineitem_table_size;

				Histogram Select_ldiscount_hist;
				copy_hist(&Select_ldiscount_hist, &ldiscount_hist);

				int Select_lpartkey = lpartkey;

				int Select_lprice = lprice;

				int Select_lquantity = lquantity;

				int Select_lshipdate = lshipdate;

				string Select_lshipinstruct = lshipinstruct;

				string Select_lshipmode = lshipmode;


				//APPLYING SUBCONDITIONS

				if(1 <= lquantity && lquantity < 24)
				{

					lineitem_table_size *= limit(&ldiscount_hist, 6, 8);

					if(34310 <= lshipdate && lshipdate < 34675)
					{



						sum_price += data_data_mult_data(1 * lineitem_table_size , data_hist_mult_data(lprice, ldiscount_hist));
					}
				}
				//RETRIEVING ATTRIBUTES

				lineitem_table_size= Select_lineitem_table_size;

				copy_hist(&ldiscount_hist, &Select_ldiscount_hist);
				lpartkey = Select_lpartkey;

				lprice = Select_lprice;

				lquantity = Select_lquantity;

				lshipdate = Select_lshipdate;

				lshipinstruct = Select_lshipinstruct;

				lshipmode = Select_lshipmode;


			}
		}
	}

}
