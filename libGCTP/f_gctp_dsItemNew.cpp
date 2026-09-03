/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      178053
Version:     1.0
Date:        2022-08-15 14:43:06
Description: 数据集字段新增
**************************************************/

#include "CDynaTable.h"

BM2_FUNCTION_EXPORT
int f_gctp_dsItemNew(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmName)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	int iSeqNo = 0;
	int iSeqNo1 = 0;
	int iSeqNo2 = 0;
	int iSeqNo3 = 0;

	//数据库操作类定义
	CDbCommand cmd(conn);

	//动态数据表定义
	CDynaTable tgctp02("TGCTP02", conn);
	CDynaTable tgctp03("TGCTP03", conn);

	try
	{
		PrintLog("cfgitmName", cfgitmName);

		if (cfgitmName != "" && bcls_rec->Tables.Contains("DS_ITEM") && bcls_rec->Tables["DS_ITEM"].Rows.get_Count() > 0)
		{
			//PrintDataTable(bcls_rec->Tables["DS_ITEM"]);

			for (int j = 0; j < bcls_rec->Tables["DS_ITEM"].Rows.get_Count(); j++)
			{
				if (bcls_rec->Tables["DS_ITEM"].Rows[j]["CFGITM_NAME"].ToString().Trim() != "" &&
					bcls_rec->Tables["DS_ITEM"].Rows[j]["CFGITM_NAME"].ToString() != cfgitmName)
				{
					continue;
				}

				tgctp03.MergeFrom(bcls_rec->Tables["DS_ITEM"].Rows[j]);
				tgctp03.SetColVal("CFGITM_NAME", cfgitmName);
				tgctp03.SetColVal("TIMESTAMP", GetTrackSeqNo("GCTP_SEQ_ID", 18, conn));

				if (tgctp03.GetColValDecimal("SEQ_NO") == 0)
				{
					iSeqNo++;
					tgctp03.SetColVal("SEQ_NO", (CDecimal)iSeqNo);
				}

				if (tgctp03.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tgctp02.MergeFrom(tgctp03.GetDataRow());

				PrintLog("DS_ITEM.TIMESTAMP", bcls_rec->Tables["DS_ITEM"].Rows[j]["TIMESTAMP"].ToString());

				if (tgctp03.GetColValString("DEFAULT_SHOW_STATE").GetLength() > 0 && tgctp03.GetColValString("DEFAULT_SHOW_STATE").Substring(0, 1) == "1")
				{
					PrintLog("条件字段", tgctp03.GetColValString("COLUMN_NAME"));

					if (bcls_rec->Tables.Contains("CONDITION_ITEM") && bcls_rec->Tables["CONDITION_ITEM"].Rows.get_Count() > 0)
					{
						PrintLog("111");
						for (int k = 0; k < bcls_rec->Tables["CONDITION_ITEM"].Rows.get_Count(); k++)
						{
							if (bcls_rec->Tables["CONDITION_ITEM"].Rows[k]["TIMESTAMP"].ToString() == bcls_rec->Tables["DS_ITEM"].Rows[j]["TIMESTAMP"].ToString())
							{
								tgctp02.MergeFrom(bcls_rec->Tables["CONDITION_ITEM"].Rows[k]);
								tgctp02.SetColVal("TIMESTAMP", tgctp03.GetColValString("TIMESTAMP"));
								tgctp02.SetColVal("CFGITM_NAME", cfgitmName);
								tgctp02.SetColVal("CFGITM_GRP_NAME", "CONDITION_ITEM");

								if (tgctp02.Insert() < 0)
								{
									throw CApplicationException(-1, s.msg, s.svc_name);
								}
							}
						}
					}
					else
					{
						PrintLog("222");
						if (bcls_rec->Tables.Contains("CONDITION_ITEM_CONFIG"))
						{
							for (int k = 0; k < bcls_rec->Tables["CONDITION_ITEM_CONFIG"].Rows.get_Count(); k++)
							{
								CString colName = bcls_rec->Tables["CONDITION_ITEM_CONFIG"].Rows[k]["ITEM_ENAME"].ToString();
								PrintLog("条件配置列", colName);

								if (bcls_rec->Tables["CONDITION_ITEM_CONFIG"].Rows[k]["DEFAULT_VALUE"].ToString().Trim() != "" && tgctp02.GetColValString(colName).Trim() == "")
								{
									PrintLog("DEFAULT_VALUE", bcls_rec->Tables["CONDITION_ITEM_CONFIG"].Rows[k]["DEFAULT_VALUE"].ToString().Trim());
									tgctp02.SetColVal(colName, bcls_rec->Tables["CONDITION_ITEM_CONFIG"].Rows[k]["DEFAULT_VALUE"].ToString().Trim());
								}
							}
						}

						tgctp02.SetColVal("CFGITM_GRP_NAME", "CONDITION_ITEM");

						if (tgctp02.GetColValDecimal("SEQ_NO_01") == 0)
						{
							iSeqNo1++;
							tgctp02.SetColVal("SEQ_NO_01", (CDecimal)iSeqNo1);
						}

						tgctp02.CopyColVal("LABEL_TEXT", "COLUMN_CNAME");

						if (tgctp02.GetColValString("DATA_TYPE") == "N")
						{
							tgctp02.SetColVal("CONTROL_CLASS", "05");
							tgctp02.SetColVal("OPERATOR", "4");
						}
						else
						{
							if (tgctp02.GetColValString("DATA_TYPE") == "O" || tgctp02.GetColValString("DATA_TYPE") == "H" ||
								tgctp02.GetColValString("DATA_TYPE") == "Z")
							{
								tgctp02.SetColVal("CONTROL_CLASS", "02");
								tgctp02.SetColVal("OPERATOR", "0");
							}
							else if (tgctp02.GetColValString("DATA_TYPE") == "B")
							{
								tgctp02.SetColVal("CONTROL_CLASS", "09");
								tgctp02.SetColVal("OPERATOR", "0");
							}
							else if (tgctp02.GetColValString("DATA_TYPE") == "D")
							{
								tgctp02.SetColVal("CONTROL_CLASS", "06");
								tgctp02.SetColVal("OPERATOR", "4");
							}
							else
							{
								tgctp02.SetColVal("CONTROL_CLASS", "01");
								tgctp02.SetColVal("OPERATOR", "0");
							}
						}

						if (tgctp02.Insert() < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
				}

				tgctp02.InitDataRow();
				tgctp02.MergeFrom(tgctp03.GetDataRow());

				if (tgctp03.GetColValString("DEFAULT_SHOW_STATE").GetLength() > 1 && tgctp03.GetColValString("DEFAULT_SHOW_STATE").Substring(1, 1) == "1")
				{
					PrintLog("Grid字段", tgctp03.GetColValString("COLUMN_NAME"));

					if (bcls_rec->Tables.Contains("GRID_ITEM") && bcls_rec->Tables["GRID_ITEM"].Rows.get_Count() > 0)
					{
						for (int k = 0; k < bcls_rec->Tables["GRID_ITEM"].Rows.get_Count(); k++)
						{
							if (bcls_rec->Tables["GRID_ITEM"].Rows[k]["TIMESTAMP"].ToString() == bcls_rec->Tables["DS_ITEM"].Rows[j]["TIMESTAMP"].ToString())
							{
								tgctp02.MergeFrom(bcls_rec->Tables["GRID_ITEM"].Rows[k]);
								tgctp02.SetColVal("TIMESTAMP", tgctp03.GetColValString("TIMESTAMP"));
								tgctp02.SetColVal("CFGITM_NAME", cfgitmName);
								tgctp02.SetColVal("CFGITM_GRP_NAME", "GRID_ITEM");

								break;
							}
						}
					}
					else
					{
						tgctp02.SetColVal("CFGITM_GRP_NAME", "GRID_ITEM");
						tgctp02.SetColVal("MAIN_FLAG", "0");

						if (tgctp02.GetColValDecimal("SEQ_NO_02") == 0)
						{
							iSeqNo2++;
							tgctp02.SetColVal("SEQ_NO_02", (CDecimal)iSeqNo2);
						}

						if (bcls_rec->Tables.Contains("GRID_ITEM_CONFIG"))
						{
							for (int k = 0; k < bcls_rec->Tables["GRID_ITEM_CONFIG"].Rows.get_Count(); k++)
							{
								CString colName = bcls_rec->Tables["GRID_ITEM_CONFIG"].Rows[k]["ITEM_ENAME"].ToString();
								PrintLog("Grid配置列", colName);
								tgctp02.Print(colName);

								if (bcls_rec->Tables["GRID_ITEM_CONFIG"].Rows[k]["DEFAULT_VALUE"].ToString().Trim() != "" && tgctp02.GetColValString(colName).Trim() == "")
								{
									PrintLog("DEFAULT_VALUE", bcls_rec->Tables["GRID_ITEM_CONFIG"].Rows[k]["DEFAULT_VALUE"].ToString().Trim());
									tgctp02.SetColVal(colName, bcls_rec->Tables["GRID_ITEM_CONFIG"].Rows[k]["DEFAULT_VALUE"].ToString().Trim());
								}
							}
						}
					}

					//tgctp02.Print();
					if (tgctp02.Insert() < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}

				tgctp02.InitDataRow();
				tgctp02.MergeFrom(tgctp03.GetDataRow());

				if (tgctp03.GetColValString("DEFAULT_SHOW_STATE").GetLength() > 2 && tgctp03.GetColValString("DEFAULT_SHOW_STATE").Substring(2, 1) == "1")
				{
					PrintLog("控件字段", tgctp03.GetColValString("COLUMN_NAME"));

					if (bcls_rec->Tables.Contains("CONTROL_ITEM") && bcls_rec->Tables["CONTROL_ITEM"].Rows.get_Count() > 0)
					{
						//PrintDataTable(bcls_rec->Tables["CONTROL_ITEM"]);

						for (int k = 0; k < bcls_rec->Tables["CONTROL_ITEM"].Rows.get_Count(); k++)
						{
							if (bcls_rec->Tables["CONTROL_ITEM"].Rows[k]["TIMESTAMP"].ToString() == bcls_rec->Tables["DS_ITEM"].Rows[j]["TIMESTAMP"].ToString())
							{
								PrintLog("匹配,k", k);
								tgctp03.Print("TIMESTAMP");

								tgctp02.MergeFrom(bcls_rec->Tables["CONTROL_ITEM"].Rows[k]);
								tgctp02.SetColVal("TIMESTAMP", tgctp03.GetColValString("TIMESTAMP"));
								tgctp02.SetColVal("CFGITM_NAME", cfgitmName);
								tgctp02.SetColVal("CFGITM_GRP_NAME", "CONTROL_ITEM");

								break;
							}
						}
					}
					else
					{
						if (bcls_rec->Tables.Contains("CONTROL_ITEM_CONFIG"))
						{
							for (int k = 0; k < bcls_rec->Tables["CONTROL_ITEM_CONFIG"].Rows.get_Count(); k++)
							{
								CString colName = bcls_rec->Tables["CONTROL_ITEM_CONFIG"].Rows[k]["ITEM_ENAME"].ToString();
								PrintLog("Grid配置列", colName);

								if (bcls_rec->Tables["CONTROL_ITEM_CONFIG"].Rows[k]["DEFAULT_VALUE"].ToString().Trim() != "" && tgctp02.GetColValString(colName).Trim() == "")
								{
									PrintLog("DEFAULT_VALUE", bcls_rec->Tables["CONTROL_ITEM_CONFIG"].Rows[k]["DEFAULT_VALUE"].ToString().Trim());
									tgctp02.SetColVal(colName, bcls_rec->Tables["CONTROL_ITEM_CONFIG"].Rows[k]["DEFAULT_VALUE"].ToString().Trim());
								}
							}
						}

						tgctp02.SetColVal("CFGITM_GRP_NAME", "CONTROL_ITEM");

						if (tgctp02.GetColValDecimal("SEQ_NO_01") == 0)
						{
							iSeqNo1++;
							tgctp02.SetColVal("SEQ_NO_01", (CDecimal)iSeqNo1);
						}

						tgctp02.CopyColVal("LABEL_TEXT", "COLUMN_CNAME");

						if (tgctp02.GetColValString("DATA_TYPE") == "N")
						{
							tgctp02.SetColVal("CONTROL_CLASS", "05");
						}
						else
						{
							if (tgctp02.GetColValString("DATA_TYPE") == "O" || tgctp02.GetColValString("DATA_TYPE") == "H" ||
								tgctp02.GetColValString("DATA_TYPE") == "Z")
							{
								tgctp02.SetColVal("CONTROL_CLASS", "02");
							}
							else if (tgctp02.GetColValString("DATA_TYPE") == "B")
							{
								tgctp02.SetColVal("CONTROL_CLASS", "09");
							}
							else if (tgctp02.GetColValString("DATA_TYPE") == "D")
							{
								tgctp02.SetColVal("CONTROL_CLASS", "06");
							}
							else
							{
								tgctp02.SetColVal("CONTROL_CLASS", "01");
							}
						}
					}

					//tgctp02.Print();
					if (tgctp02.Insert() < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
			}
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}