
#include "database_preparation.h"

using namespace std;

	double sum_price = 0;

int main(){


	int lineitem_ia[] = {l_discount_id, l_price_id, l_shipdate_id, l_quantity_id};
	int lineitem_sa[] = {l_shipinstruct_id, l_shipmode_id};
    read_lineitems_from_file("edited_lineitem.tbl", lineitem_ia, 4, lineitem_sa, 2);
    read_histogram_defaults_from_file("histograms.txt");
	//SCANNING ATTRIBUTES(TAKING A COPY FROM DEFAULTS)

	{
		Histogram l_discount_hist;
		l_discount_hist.minimum = default_histogram[lineitem][l_discount_id].minimum;
		l_discount_hist.maximum = default_histogram[lineitem][l_discount_id].maximum;
		l_discount_hist.bucket_count = default_histogram[lineitem][l_discount_id].bucket_count;
		l_discount_hist.bucket_size = default_histogram[lineitem][l_discount_id].bucket_size;
		for (int i = 0; i < l_discount_hist.bucket_count; ++i){
		    l_discount_hist.bucket[i] = default_histogram[lineitem][l_discount_id].bucket[i];
		}
		for (int lineitem_it = 0; lineitem_it < table_size[lineitem]; lineitem_it++)
		{
			int l_partkey = int_table[lineitem][l_partkey_id][lineitem_it];

			int l_price = int_table[lineitem][l_price_id][lineitem_it];

			int l_quantity = int_table[lineitem][l_quantity_id][lineitem_it];

			int l_shipdate = int_table[lineitem][l_shipdate_id][lineitem_it];

			string l_shipinstruct = string_table[lineitem][l_shipinstruct_id][lineitem_it];

			string l_shipmode = string_table[lineitem][l_shipmode_id][lineitem_it];

			double lineitem_table_size = 1;
			//APPLYING THE CONDITIONS OF EVERY SUBCONDITION INSIDE SELECT NODE

			{
				//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION

				int Select_lineitem_table_size = lineitem_table_size;

				Histogram Select_l_discount_hist;
				copy_hist(&Select_l_discount_hist, &l_discount_hist);

				int Select_l_partkey = l_partkey;

				int Select_l_price = l_price;

				int Select_l_quantity = l_quantity;

				int Select_l_shipdate = l_shipdate;

				string Select_l_shipinstruct = l_shipinstruct;

				string Select_l_shipmode = l_shipmode;


				//APPLYING SUBCONDITIONS

				if(1 <= l_quantity && l_quantity < 24)
				{

					lineitem_table_size *= limit(&l_discount_hist, 6, 8);

					if(34310 <= l_shipdate && l_shipdate < 34675)
					{



						sum_price += data_data_mult_data(1 * lineitem_table_size , data_hist_mult_data(l_price, l_discount_hist));
					}
				}
				//RETRIEVING ATTRIBUTES

				lineitem_table_size= Select_lineitem_table_size;

				copy_hist(&l_discount_hist, &Select_l_discount_hist);
				l_partkey = Select_l_partkey;

				l_price = Select_l_price;

				l_quantity = Select_l_quantity;

				l_shipdate = Select_l_shipdate;

				l_shipinstruct = Select_l_shipinstruct;

				l_shipmode = Select_l_shipmode;


			}
		}
	}

}
