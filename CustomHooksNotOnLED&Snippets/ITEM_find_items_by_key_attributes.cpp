if (projItem == NULL)
			{
				int n_items;
				tag_t* items;
				const char ** att_names=NULL;
				const char ** attr=NULL;

				att_names = (const char**)MEM_alloc(1 * sizeof(char*));
				att_names[0] = "object_name";

				attr = (const char**)MEM_alloc(1 * sizeof(char*));
				attr[0] = temp;

				rStatus = ITEM_find_items_by_key_attributes(1, att_names,attr,&n_items, &items);

				att_names[0] = "object_name2";
			}