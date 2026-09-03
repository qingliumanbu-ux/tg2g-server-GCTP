/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      admin
Version:     1.0
Date:        2018-09-20 13:18:50
Description: 获取配置数据项函数
**************************************************/

#include "CUtils2.h"

BM2_FUNCTION_EXPORT
int f_gctp_getConfigItem(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn, CString cfgitmGrpName, CString custFlag)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	list<CString> listCodeClass;

	//数据库操作类定义
	CDbCommand cmd_inq(conn);

	try
	{
		if (!bcls_ret->Tables.Contains(cfgitmGrpName))
		{
			bcls_ret->Tables.Add(cfgitmGrpName);
		}

		sqlstr = "SELECT * FROM tgctp00 WHERE cfgitm_grp_name = '" + cfgitmGrpName + "'";
		if (custFlag.Trim() != "")
		{
			sqlstr += " AND cust_flag = '" + custFlag + "'";
		}
		sqlstr += " ORDER BY seq_no";

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		cmd_inq.ExecuteQuery(bcls_ret->Tables[cfgitmGrpName]);
		cmd_inq.Close();

		for (int i = 0; i < bcls_ret->Tables[cfgitmGrpName].Columns.get_Count(); i++)
		{
			if (bcls_ret->Tables[cfgitmGrpName].Columns[i].get_DataType() != DT_STRING &&
				bcls_ret->Tables[cfgitmGrpName].Columns[i].get_DataType() != DT_DECIMAL)
			{
				CString colName = bcls_ret->Tables[cfgitmGrpName].Columns[i].get_ColumnName();
				PrintLog("colName", colName);
				bcls_ret->Tables[cfgitmGrpName].Columns.Add(DT_DECIMAL, colName + "_N");

				for (int j = 0; j < bcls_ret->Tables[cfgitmGrpName].Rows.get_Count(); j++)
				{
					bcls_ret->Tables[cfgitmGrpName].Rows[j][colName + "_N"] = bcls_ret->Tables[cfgitmGrpName].Rows[j][colName];
				}

				bcls_ret->Tables[cfgitmGrpName].Columns[colName].Delete();
				bcls_ret->Tables[cfgitmGrpName].Columns[colName + "_N"].set_ColumnName(colName);
			}
		}

		for (int i = 0; i < bcls_ret->Tables[cfgitmGrpName].Rows.get_Count(); i++)
		{
			CString codeClass = bcls_ret->Tables[cfgitmGrpName].Rows[i]["CODE_CLASS"].ToString().Trim();

			if (codeClass != "" && !bcls_ret->Tables.Contains(codeClass))
			{
				PrintLog("codeClass", codeClass);

				bool existsFlag = false;
				for (list<CString>::const_iterator iter = listCodeClass.begin(); iter != listCodeClass.end(); iter++)
				{
					if (*iter == codeClass)
					{
						existsFlag = true;
						break;
					}
				}

				if (bcls_ret->Tables.Contains("CODE_CLASS"))
				{
					PrintLog("exists CODE_CLASS");
					for (int j = 0; j < bcls_ret->Tables["CODE_CLASS"].Rows.get_Count(); j++)
					{
						if (bcls_ret->Tables["CODE_CLASS"].Rows[j]["CODE_CLASS"].ToString() == codeClass)
						{
							existsFlag = true;
							break;
						}
					}
				}

				if (existsFlag == false)
				{
					listCodeClass.push_back(codeClass);
					PrintLog("push_back blkName", codeClass);
				}
			}
		}

		if (listCodeClass.size() > 0)
		{
			if (!bcls_ret->Tables.Contains("CODE_CLASS"))
			{
				bcls_ret->Tables.Add("CODE_CLASS");
				bcls_ret->Tables["CODE_CLASS"].Columns.Add(DT_STRING, "CODE_CLASS");
				bcls_ret->Tables["CODE_CLASS"].Columns.Add(DT_STRING, "CODE");
				bcls_ret->Tables["CODE_CLASS"].Columns.Add(DT_STRING, "CODE_DESC_1_CONTENT");
			}
			
			CDataTable dtCode;
			sqlstr = "SELECT code_class,code,code_desc_1_content FROM tep0002 WHERE code_class IN (";
			for (list<CString>::const_iterator iter = listCodeClass.begin(); iter != listCodeClass.end(); iter++)
			{
				if (iter != listCodeClass.begin())
				{
					sqlstr += ",";
				}
				sqlstr += "'" + *iter + "'";
			}

			sqlstr += ") ORDER BY code_class,code";
			PrintLog("sqlstr", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(dtCode);
			cmd_inq.Close();

			for (int i = 0; i < dtCode.Rows.get_Count(); i++)
			{
				bcls_ret->Tables["CODE_CLASS"].Rows.Add();
				bcls_ret->Tables["CODE_CLASS"].Rows[bcls_ret->Tables["CODE_CLASS"].Rows.get_Count() - 1].Merge(dtCode.Rows[i]);
			}
			
			//PrintDataTable(bcls_ret->Tables["CODE_CLASS"]);
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
int f_gctp_getConfigItem(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	list<CString> listCfgitmGrpName;

	try
	{
		listCfgitmGrpName.push_back("DS_MAIN");
		listCfgitmGrpName.push_back("DS_ITEM");
		listCfgitmGrpName.push_back("CONDITION_MAIN");
		listCfgitmGrpName.push_back("CONDITION_ITEM");
		listCfgitmGrpName.push_back("GRID_MAIN");
		listCfgitmGrpName.push_back("GRID_ITEM");
		listCfgitmGrpName.push_back("CONTROL_MAIN");
		listCfgitmGrpName.push_back("CONTROL_ITEM");
		listCfgitmGrpName.push_back("FORM_CFGITM");
		listCfgitmGrpName.push_back("CUSTOM_EVENT");
		listCfgitmGrpName.push_back("OPERATE_DO");
		listCfgitmGrpName.push_back("OPERATE_IN");
		listCfgitmGrpName.push_back("OPERATE_OUT");

		for (list<CString>::const_iterator iter = listCfgitmGrpName.begin(); iter != listCfgitmGrpName.end(); iter++)
		{
			doFlag = f_gctp_getConfigItem(bcls_rec, bcls_ret, conn, *iter, "");
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
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


