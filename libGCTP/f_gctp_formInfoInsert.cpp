/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-09-19 14:06:36
Description: 框架画面信息新增函数
**************************************************/

#include "CUtils.h"

BM2_FUNCTION_EXPORT
int f_gctp_formInfoInsert(CString formNo, CString formName, CString formPartition, CString appName, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDecimal dAclid = 0;
	CDbCommand cmd(conn);

	try
	{
		//CTransactionManager::Commit(0);

		sqlstr = "SELECT aclid FROM tesformresinfo WHERE name = '" + formNo + "'";
		PrintLog("sqlstr", sqlstr);
		cmd.SetCommandText(sqlstr);
		dAclid = cmd.ExecuteScalar();
		if (dAclid > 0)
		{
			//删除原注册画面和按钮信息
			CDbCommand cmd_del(conn);

			sqlstr = "DELETE FROM tesbuttonresinfo_res WHERE aclid IN"
				" (SELECT aclid FROM tesbuttonresinfo WHERE fname = @FORM_NO)";
			cmd_del.SetCommandText(sqlstr);
			cmd_del.Parameters.Set("FORM_NO", formNo);
			cmd_del.ExecuteNonQuery();
			cmd_del.Close();

			sqlstr = "DELETE FROM tesbuttonresinfo WHERE fname = @FORM_NO";
			cmd_del.SetCommandText(sqlstr);
			cmd_del.ExecuteNonQuery();
			cmd_del.Close();

			sqlstr = "DELETE FROM tesformresinfo_res WHERE aclid IN"
				" (SELECT aclid FROM tesformresinfo WHERE name = @FORM_NO)";
			cmd_del.SetCommandText(sqlstr);
			cmd_del.ExecuteNonQuery();
			cmd_del.Close();

			sqlstr = "DELETE FROM tesformresinfo WHERE name = @FORM_NO";
			cmd_del.SetCommandText(sqlstr);
			cmd_del.ExecuteNonQuery();
			cmd_del.Close();
		}
		cmd.Close();

		//新增画面注册配置
		dAclid = 1000000000 + GetSeqence("MMTP_FORM_SEQ_NO", conn);
		PrintLog("dAclid", dAclid);

		//TESFORMRESINFO
		sqlstr = "INSERT INTO tesformresinfo VALUES('" + formNo + "','" + formName + "','GCTP.DLL'," + dAclid.ToString() + 
			",'" + formPartition + "',0,'1','" + appName + "',' ','" + (CString)s.userid + "',' ',0)";
		PrintLog("sqlstr", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteNonQuery();
		cmd.Close();

		//TESFORMRESINFO_RES
		sqlstr = "INSERT INTO tesformresinfo_res VALUES('zh_Hans'," + dAclid.ToString() + ", '" + formName + "')";
		PrintLog("sqlstr", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteNonQuery();
		cmd.Close();

		//子母画面配置数据已存在则先删除
		sqlstr = "DELETE FROM tesformpara WHERE form_name = '" + formNo + "'";
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteNonQuery();
		cmd.Close();

		//新增子母画面配置数据
		sqlstr = "INSERT INTO tesformpara (REC_CREATOR,REC_CREATE_TIME,FORM_NAME,FORM_BASE_NAME) VALUES ('" +
			(CString)s.userid + "','" + (CString)s.datetime + "','" + formNo + "','" + "GCTPFORM1')";
		PrintLog("sqlstr", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteNonQuery();
		cmd.Close();
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

	//if (doFlag == 0)
	//{
	//	CTransactionManager::Commit(0);
	//	CTransactionManager::Begin(0, 0);
	//}
	//else
	//{
	//	CTransactionManager::Abort(0);
	//	CTransactionManager::Begin(0, 0);
	//}

	return doFlag;
}


