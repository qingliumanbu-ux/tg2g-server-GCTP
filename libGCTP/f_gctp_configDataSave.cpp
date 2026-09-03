#include "CDynaTable2.h"

BM2_FUNCTION_IMPORT
int f_gctp_getConfigItem(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmGrpName, CString custFlag);

BM2_FUNCTION_IMPORT
int f_gctp_getConfigData(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2_FUNCTION_IMPORT
int f_gctp_buttonConfig(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString formNo);

BM2_FUNCTION_IMPORT
int f_gctp_setCfgDataVt(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmName, CString cfgitmGrpName);

BM2_FUNCTION_IMPORT
int f_gctp_dsItemNew(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmName);

BM2_FUNCTION_EXPORT
int f_gctp_configDataSave(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmName)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDecimal dAclid = 0;
	int iRowNoTp02 = 0;

	//动态数据表定义
	CDynaTable2 tgctp01("TGCTP01", conn);
	CDynaTable2 tgctp02("TGCTP02", conn);
	CDynaTable2 tgctp03("TGCTP02", conn);

	//数据库操作类定义
	CDbCommand cmd(conn);
	CDbCommand cmd_upd(conn);
	CDbCommand cmd_ins(conn);

	try
	{
		if (cfgitmName.Trim() != "")
		{
			if (bcls_rec->Tables.Contains("DS_ITEM") && bcls_rec->Tables["DS_ITEM"].Rows.get_Count() > 0)
			{
				doFlag = f_gctp_dsItemNew(bcls_rec, bcls_ret, conn, cfgitmName);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else
			{
				if (bcls_rec->Tables.Contains("CONDITION_ITEM") && bcls_rec->Tables["CONDITION_ITEM"].Rows.get_Count() > 0)
				{
					PrintLog("CONDITION_ITEM");
					doFlag = f_gctp_getConfigItem(bcls_rec, bcls_ret, conn, "CONDITION_ITEM", "");

					tgctp02.AddUpdateColName("SEQ_NO_01");
					for (int j = 0; j < bcls_ret->Tables["CONDITION_ITEM"].Rows.get_Count(); j++)
					{
						CString colName = bcls_ret->Tables["CONDITION_ITEM"].Rows[j]["ITEM_ENAME"].ToString();
						tgctp02.AddUpdateColName(colName);
					}

					tgctp02.ClearDataRow();
					CDecimal seqNo1 = 0;
					CString sqlWhere = "";
					for (int j = 0; j < bcls_rec->Tables["CONDITION_ITEM"].Rows.get_Count(); j++)
					{
						if (GetColValueC(bcls_rec->Tables["CONDITION_ITEM"], j, "CFGITM_NAME").Trim() == "" ||
							GetColValueC(bcls_rec->Tables["CONDITION_ITEM"], j, "CFGITM_NAME") == cfgitmName)
						{
							tgctp02.MergeFrom(bcls_rec->Tables["CONDITION_ITEM"].Rows[j]);
							tgctp02.SetColVal("CFGITM_GRP_NAME", "CONDITION_ITEM");

							seqNo1 = seqNo1 + 1;
							tgctp02.SetColVal("SEQ_NO_01", seqNo1);

							//tgctp02.Print();

							if (tgctp02.GetColValString("TIMESTAMP").Trim() == "")
							{
								PrintLog("Insert");
								tgctp02.SetColVal("TIMESTAMP", GetTrackSeqNo("GCTP_SEQ_ID", 18, conn));
								tgctp02.SetColVal("DEFAULT_SHOW_STATE", "100");

								if (tgctp02.Insert() < 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
							}
							else if (tgctp02.GetColValString("TIMESTAMP").Substring(0, 1) == "M")
							{
								tgctp02.SetColVal("TIMESTAMP", tgctp02.GetColValString("TIMESTAMP").Substring(1));
								tgctp02.SetColVal("DEFAULT_SHOW_STATE", "100");

								if (tgctp02.Insert() < 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
							}
							else
							{
								PrintLog("Update");
								if (tgctp02.Update() < 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
							}

							if (sqlWhere.Trim() != "")
							{
								sqlWhere += ",";
							}

							sqlWhere += "'" + tgctp02.GetColValString("TIMESTAMP") + "'";

						}
					}

					if (sqlWhere.Trim() != "")
					{
						sqlstr = "DELETE FROM tgctp02 WHERE cfgitm_name = '" + cfgitmName +
							"' AND cfgitm_grp_name = 'CONDITION_ITEM' AND timestamp NOT IN(" + sqlWhere + ")";
						PrintLog("sqlstr", sqlstr);
						cmd.SetCommandText(sqlstr);
						cmd.ExecuteNonQuery();
						cmd.Close();
					}
				}

				if (bcls_rec->Tables.Contains("GRID_ITEM") && bcls_rec->Tables["GRID_ITEM"].Rows.get_Count() > 0)
				{
					PrintLog("GRID_ITEM");
					doFlag = f_gctp_getConfigItem(bcls_rec, bcls_ret, conn, "GRID_ITEM", "");

					tgctp02.AddUpdateColName("SEQ_NO_02");
					for (int j = 0; j < bcls_ret->Tables["GRID_ITEM"].Rows.get_Count(); j++)
					{
						CString colName = bcls_ret->Tables["GRID_ITEM"].Rows[j]["ITEM_ENAME"].ToString();
						tgctp02.AddUpdateColName(colName);
					}

					tgctp02.ClearDataRow();
					for (int j = 0; j < bcls_rec->Tables["GRID_ITEM"].Rows.get_Count(); j++)
					{
						if (GetColValueC(bcls_rec->Tables["GRID_ITEM"], j, "CFGITM_NAME").Trim() == "" ||
							GetColValueC(bcls_rec->Tables["GRID_ITEM"], j, "CFGITM_NAME") == cfgitmName)
						{
							tgctp02.MergeFrom(bcls_rec->Tables["GRID_ITEM"].Rows[j], iRowNoTp02++);
						}
					}

					for (int j = 0; j < tgctp02.GetRowCount(); j++)
					{
						tgctp02.SetColVal("CFGITM_GRP_NAME", "GRID_ITEM", j);
						tgctp02.SetColVal("SEQ_NO_02", (CDecimal)j + 1, j);
					}

					if (tgctp02.Update() < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}

				if (bcls_rec->Tables.Contains("CONTROL_ITEM") && bcls_rec->Tables["CONTROL_ITEM"].Rows.get_Count() > 0)
				{
					PrintLog("CONTROL_ITEM");
					doFlag = f_gctp_getConfigItem(bcls_rec, bcls_ret, conn, "CONTROL_ITEM", "");

					tgctp02.AddUpdateColName("SEQ_NO_03");
					for (int j = 0; j < bcls_ret->Tables["CONTROL_ITEM"].Rows.get_Count(); j++)
					{
						CString colName = bcls_ret->Tables["CONTROL_ITEM"].Rows[j]["ITEM_ENAME"].ToString();
						tgctp02.AddUpdateColName(colName);
					}

					tgctp02.ClearDataRow();
					CDecimal seqNo3 = 0;
					for (int j = 0; j < bcls_rec->Tables["CONTROL_ITEM"].Rows.get_Count(); j++)
					{
						if (GetColValueC(bcls_rec->Tables["CONTROL_ITEM"], j, "CFGITM_NAME").Trim() == "" ||
							GetColValueC(bcls_rec->Tables["CONTROL_ITEM"], j, "CFGITM_NAME") == cfgitmName)
						{
							tgctp02.MergeFrom(bcls_rec->Tables["CONTROL_ITEM"].Rows[j]);
							tgctp02.SetColVal("CFGITM_GRP_NAME", "CONTROL_ITEM");

							seqNo3 = seqNo3 + 1;
							tgctp02.SetColVal("SEQ_NO_03", seqNo3);

							if (tgctp02.GetColValString("TIMESTAMP").Trim() == "")
							{
								tgctp02.SetColVal("TIMESTAMP", GetTrackSeqNo("GCTP_SEQ_ID", 18, conn));
								tgctp02.SetColVal("DEFAULT_SHOW_STATE", "001");

								if (tgctp02.Insert() < 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
							}
							else
							{
								if (tgctp02.Update() < 0)
								{
									throw CApplicationException(-1, s.msg, log.Location);
								}
							}
						}
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

BM2_FUNCTION_EXPORT
int f_gctp_configDataSave(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CString cfgitmName = "";

	//数据库操作类定义
	CDbCommand cmd(conn);

	try
	{
		cfgitmName = GetColValueC(bcls_rec->Tables[0], 0, "CFGITM_NAME").Trim();
		if (cfgitmName == "")
		{
			strcpy(s.msg, "没有传入配置名");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		doFlag = f_gctp_setCfgDataVt(bcls_rec, bcls_ret, conn, cfgitmName, "DS_MAIN");
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		doFlag = f_gctp_setCfgDataVt(bcls_rec, bcls_ret, conn, cfgitmName, "CONDITION_MAIN");
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		
		doFlag = f_gctp_setCfgDataVt(bcls_rec, bcls_ret, conn, cfgitmName, "GRID_MAIN");
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		doFlag = f_gctp_setCfgDataVt(bcls_rec, bcls_ret, conn, cfgitmName, "CONTROL_MAIN");
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		doFlag = f_gctp_configDataSave(bcls_rec, bcls_ret, conn, cfgitmName);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
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

