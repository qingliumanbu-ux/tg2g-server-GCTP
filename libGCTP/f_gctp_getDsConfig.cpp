/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      admin
Version:     1.0
Date:        2022-07-08 16:55:47
Description: 获取数据源配置
**************************************************/

#include "CUtils.h"

BM2_FUNCTION_IMPORT
int f_gctp_getCfgDataVt(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmName, CString cfgitmGrpName);

BM2_FUNCTION_EXPORT
int f_gctp_getDsConfig(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CString dsName = "";

	//数据库操作类定义
	CDbCommand cmd_inq(conn);

	try
	{
		dsName = bcls_rec->Tables[0].Rows[0]["DATASET_NAME"].ToString();
		PrintLog("dsName", dsName);

		//查询数据源主项配置
		doFlag = f_gctp_getCfgDataVt(bcls_rec, bcls_ret, conn, dsName, "DS_MAIN");
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//查询数据源字段配置
		bcls_ret->Tables.Add("DS_ITEM");
		sqlstr = "SELECT * FROM tgctp03 WHERE dataset_name = '" + dsName + "' ORDER BY seq_no";
		PrintLog("sqlstr", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables["DS_ITEM"]);
		cmd_inq.Close();
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


