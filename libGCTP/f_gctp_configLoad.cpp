/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-09-19 14:06:36
Description: 配置加载函数
**************************************************/

#include "CUtils.h"

BM2_FUNCTION_EXPORT
int f_gctp_configLoad(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn, int loadFlag)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDbCommand cmd_inq(conn);

	try
	{
		if (loadFlag == 0)
		{
			sqlstr = "SELECT DISTINCT cfgitm_name,cfggrp_name FROM tgctp01";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

			bcls_ret->Tables[0].set_TableName("CFGITM_NAME");

			bcls_ret->Tables.Add("TMMTP01");
			SetDataTableColName("TMMTP01", bcls_ret->Tables["TMMTP01"], conn);

			bcls_ret->Tables.Add("TMMTP02");
			SetDataTableColName("TMMTP02", bcls_ret->Tables["TMMTP02"], conn);

			bcls_ret->Tables.Add("TMMTP03");
			SetDataTableColName("TMMTP03", bcls_ret->Tables["TMMTP03"], conn);

			bcls_ret->Tables.Add("TMMTP05");
			SetDataTableColName("TMMTP05", bcls_ret->Tables["TMMTP05"], conn);
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


