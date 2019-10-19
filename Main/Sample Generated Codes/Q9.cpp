
#include "database_preparation.h"

using namespace std;

	double sum_price = 0;
	double sum_disc_price = 0;

int main(){

    int lineitem_ia[] = {ldiscount_id, lpartkey_id, lprice_id, lquantity_id};
    int lineitem_sa[] = {lshipinstruct_id, lshipmode_id};
    read_lineitems_from_file("edited_lineitem.tbl", lineitem_ia, 4, lineitem_sa, 2);

    int part_ia[] = {ppartkey_id, psize_id};
    int part_sa[] = {pcontainer_id, pbrand_id};
    read_part_from_file("edited_part.tbl", part_ia, 2, part_sa, 2);
    read_histogram_defaults_from_file("histograms.txt");


        for (int i = 0; i < table_size[part]; ++i) {
        insert(ppartkey_hash_id, int_table[part][ppartkey_id][i], i);
    }

	//SCANNING ATTRIBUTES(TAKING A COPY FROM DEFAULTS)

	{
		for (int lineitem_it = 0; lineitem_it < table_size[lineitem]; lineitem_it++)
		{
			int lpartkey = int_table[lineitem][lpartkey_id][lineitem_it];

			int lprice = int_table[lineitem][lprice_id][lineitem_it];

			int lquantity = int_table[lineitem][lquantity_id][lineitem_it];

			string lshipinstruct = string_table[lineitem][lshipinstruct_id][lineitem_it];

			string lshipmode = string_table[lineitem][lshipmode_id][lineitem_it];

			double lineitem_table_size = 1;
			//UNHASHING KEY ATTRIBUTE AND FINDING OTHER ATTRIBUTES IN THAT TABLE(TAKING A COPY FROM DEFAULTS)

			{
				int part_it = search(ppartkey_hash_id, lpartkey)->data;
				{
					string pbrand = string_table[part][pbrand_id][part_it];

					string pcontainer = string_table[part][pcontainer_id][part_it];

					int ppartkey = int_table[part][ppartkey_id][part_it];


					double part_table_size = 1;
					//JOIN
					{
						int Join_lineitem_table_size = lineitem_table_size;
						int Join_part_table_size = part_table_size;

						if (lpartkey == ppartkey){

							//APPLYING THE CONDITIONS OF EVERY SUBCONDITION INSIDE SELECT NODE

							{
								//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION

								int Select_lineitem_table_size = lineitem_table_size;
								int Select_part_table_size = part_table_size;

								int Select_lpartkey = lpartkey;

								int Select_lprice = lprice;

								int Select_lquantity = lquantity;

								string Select_lshipinstruct = lshipinstruct;

								string Select_lshipmode = lshipmode;

								string Select_pbrand = pbrand;

								string Select_pcontainer = pcontainer;

								int Select_ppartkey = ppartkey;


								//APPLYING SUBCONDITIONS

								if(1 <= lquantity && lquantity < 12)
								{


									if(
									   (pcontainer == "SM_CASE")||
									   (pcontainer == "SM_BOX")||
									   (pcontainer == "SM_PACK")||
									   (pcontainer == "SM_PKG")
									    ){

										if(
										   (pbrand == "Brand#12")
										    ){

											if(
											   (lshipmode == "AIR")||
											   (lshipmode == "REG_AIR")
											    ){

												if(
												   (lshipinstruct == "DELIVER_IN_PERSON")
												    ){



													sum_price += data_data_mult_data(1 * lineitem_table_size  * part_table_size , 4 + 3*Power(lprice,2));
												}
											}
										}
									}
								}
								//RETRIEVING ATTRIBUTES

								lineitem_table_size= Select_lineitem_table_size;
								part_table_size= Select_part_table_size;

								lpartkey = Select_lpartkey;

								lprice = Select_lprice;

								lquantity = Select_lquantity;

								lshipinstruct = Select_lshipinstruct;

								lshipmode = Select_lshipmode;

								pbrand = Select_pbrand;

								pcontainer = Select_pcontainer;

								ppartkey = Select_ppartkey;


							}
							{
								//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION

								int Select_lineitem_table_size = lineitem_table_size;
								int Select_part_table_size = part_table_size;

								int Select_lpartkey = lpartkey;

								int Select_lprice = lprice;

								int Select_lquantity = lquantity;

								string Select_lshipinstruct = lshipinstruct;

								string Select_lshipmode = lshipmode;

								string Select_pbrand = pbrand;

								string Select_pcontainer = pcontainer;

								int Select_ppartkey = ppartkey;


								//APPLYING SUBCONDITIONS

								if(10 <= lquantity && lquantity < 21)
								{


									if(
									   (pcontainer == "MED_BAG")||
									   (pcontainer == "MED_BOX")||
									   (pcontainer == "MED_PKG")
									    ){

										if(
										   (pbrand == "Brand#23")
										    ){

											if(
											   (lshipmode == "AIR")||
											   (lshipmode == "REG_AIR")
											    ){

												if(
												   (lshipinstruct == "DELIVER_IN_PERSON")
												    ){



													sum_price += data_data_mult_data(1 * lineitem_table_size  * part_table_size , 4 + 3*Power(lprice,2));
												}
											}
										}
									}
								}
								//RETRIEVING ATTRIBUTES

								lineitem_table_size= Select_lineitem_table_size;
								part_table_size= Select_part_table_size;

								lpartkey = Select_lpartkey;

								lprice = Select_lprice;

								lquantity = Select_lquantity;

								lshipinstruct = Select_lshipinstruct;

								lshipmode = Select_lshipmode;

								pbrand = Select_pbrand;

								pcontainer = Select_pcontainer;

								ppartkey = Select_ppartkey;


							}
							{
								//TAKING A COPY FROM EVERY ATTRIBUTE SO WE COULD RETRIEVE IT AFTER THIS SUBCONDITION EXECUTION

								int Select_lineitem_table_size = lineitem_table_size;
								int Select_part_table_size = part_table_size;

								int Select_lpartkey = lpartkey;

								int Select_lprice = lprice;

								int Select_lquantity = lquantity;

								string Select_lshipinstruct = lshipinstruct;

								string Select_lshipmode = lshipmode;

								string Select_pbrand = pbrand;

								string Select_pcontainer = pcontainer;

								int Select_ppartkey = ppartkey;


								//APPLYING SUBCONDITIONS

								if(20 <= lquantity && lquantity < 31)
								{


									if(
									   (pcontainer == "LG_CASE")||
									   (pcontainer == "LG_BOX")||
									   (pcontainer == "LG_PACK")||
									   (pcontainer == "LG_PKG")
									    ){

										if(
										   (pbrand == "Brand#34")
										    ){

											if(
											   (lshipmode == "AIR")||
											   (lshipmode == "REG_AIR")
											    ){

												if(
												   (lshipinstruct == "DELIVER_IN_PERSON")
												    ){



													sum_price += data_data_mult_data(1 * lineitem_table_size  * part_table_size , 4 + 3*Power(lprice,2));
												}
											}
										}
									}
								}
								//RETRIEVING ATTRIBUTES

								lineitem_table_size= Select_lineitem_table_size;
								part_table_size= Select_part_table_size;

								lpartkey = Select_lpartkey;

								lprice = Select_lprice;

								lquantity = Select_lquantity;

								lshipinstruct = Select_lshipinstruct;

								lshipmode = Select_lshipmode;

								pbrand = Select_pbrand;

								pcontainer = Select_pcontainer;

								ppartkey = Select_ppartkey;


							}

							}
					lineitem_table_size= Join_lineitem_table_size;
					part_table_size= Join_part_table_size;

					}
				}
			}
		}
	}

}
