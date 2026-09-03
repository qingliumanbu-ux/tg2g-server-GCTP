/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      178053
Version:     1.0
Date:        2022-08-10 10:49:57
Description: 配置数据新增
**************************************************/

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
int f_gctp_configDataNew(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
		if (bcls_rec->Tables.Contains("DS_MAIN") && bcls_rec->Tables["DS_MAIN"].Rows.get_Count() > 0)
		{
			for (int i = 0; i < bcls_rec->Tables["DS_MAIN"].Rows.get_Count(); i++)
			{
				cfgitmName = GetColValueC(bcls_rec->Tables["DS_MAIN"], i, "CFGITM_NAME").Trim();
				PrintLog("cfgitmName", cfgitmName);

				if (cfgitmName == "")
				{
					cfgitmName = GetColValueC(bcls_rec->Tables[0], 0, "CFGITM_NAME");
				}

				if (cfgitmName != "")
				{
					doFlag = f_gctp_setCfgDataVt(bcls_rec, bcls_ret, conn, cfgitmName, "DS_MAIN");
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					doFlag = f_gctp_dsItemNew(bcls_rec, bcls_ret, conn, cfgitmName);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					PrintLog("000");
				}
			}
		}
		else
		{
			strcpy(s.msg, "没有传入主配置区域");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		PrintLog("111");
		if (bcls_rec->Tables.Contains("CONDITION_MAIN") && bcls_rec->Tables["CONDITION_MAIN"].Rows.get_Count() > 0)
		{
			PrintLog("CONDITION_MAIN");

			for (int i = 0; i < bcls_rec->Tables["CONDITION_MAIN"].Rows.get_Count(); i++)
			{
				cfgitmName = GetColValueC(bcls_rec->Tables["CONDITION_MAIN"], i, "CFGITM_NAME").Trim();
				PrintLog("cfgitmName", cfgitmName);

				doFlag = f_gctp_setCfgDataVt(bcls_rec, bcls_ret, conn, cfgitmName, "CONDITION_MAIN");
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}
		else
		{
			if (!bcls_rec->Tables.Contains("CONDITION_MAIN"))
			{
				bcls_rec->Tables.Add("CONDITION_MAIN");
			}

			bcls_rec->Tables["CONDITION_MAIN"].Rows.Add();
			AddColValue(bcls_rec->Tables["CONDITION_MAIN"], 0, "ADD_CONDITION_METHOD", "0");
		}

		PrintLog("222");
		if (bcls_rec->Tables.Contains("GRID_MAIN") && bcls_rec->Tables["GRID_MAIN"].Rows.get_Count() > 0)
		{
			PrintLog("GRID_MAIN");

			for (int i = 0; i < bcls_rec->Tables["GRID_MAIN"].Rows.get_Count(); i++)
			{
				cfgitmName = GetColValueC(bcls_rec->Tables["GRID_MAIN"], i, "CFGITM_NAME").Trim();
				PrintLog("cfgitmName", cfgitmName);

				doFlag = f_gctp_setCfgDataVt(bcls_rec, bcls_ret, conn, cfgitmName, "GRID_MAIN");
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}
		else
		{
			if (!bcls_rec->Tables.Contains("GRID_MAIN"))
			{
				bcls_rec->Tables.Add("GRID_MAIN");
			}

			bcls_rec->Tables["GRID_MAIN"].Rows.Add();
			AddColValue(bcls_rec->Tables["GRID_MAIN"], 0, "ADD_GRID_METHOD", "0");
		}

		PrintLog("333");
		if (bcls_rec->Tables.Contains("CONTROL_MAIN") && bcls_rec->Tables["CONTROL_MAIN"].Rows.get_Count() > 0)
		{
			PrintLog("CONTROL_MAIN");

			for (int i = 0; i < bcls_rec->Tables["CONTROL_MAIN"].Rows.get_Count(); i++)
			{
				cfgitmName = GetColValueC(bcls_rec->Tables["CONTROL_MAIN"], i, "CFGITM_NAME").Trim();
				PrintLog("cfgitmName", cfgitmName);

				doFlag = f_gctp_setCfgDataVt(bcls_rec, bcls_ret, conn, cfgitmName, "CONTROL_MAIN");
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}
		else
		{
			if (!bcls_rec->Tables.Contains("CONTROL_MAIN"))
			{
				bcls_rec->Tables.Add("CONTROL_MAIN");
			}

			bcls_rec->Tables["CONTROL_MAIN"].Rows.Add();
			AddColValue(bcls_rec->Tables["CONTROL_MAIN"], 0, "ADD_CONTROL_METHOD", "0");
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


