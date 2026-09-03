/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-09-19 14:06:36
Description: 框架画面按钮新增函数
**************************************************/

#include "CDynaTable.h"

BM2_FUNCTION_EXPORT
int f_gctp_formBtnInsert(CString formNo, CString funcDiv, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDynaTable buttonInfo("TESBUTTONRESINFO", conn);

	//数据库操作类定义
	CDbCommand cmd(conn);

	try
	{
		sqlstr = "SELECT appname FROM tesformresinfo WHERE name = '" + formNo + "'";
		PrintLog("sqlstr", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			if (GetColValueC(bcls_rec->Tables[i], 0, "FUNC_DIV") == "0" ||
				GetColValueC(bcls_rec->Tables[i], 0, "FUNC_DIV") == "1" ||
				GetColValueC(bcls_rec->Tables[i], 0, "FUNC_DIV") == "2")
			{
				//新增按钮注册配置
				//F2 查询按钮
				dAclid = 1000000000 + GetSeqence("MMTP_BUTTON_SEQ_NO", conn);
				PrintLog("dAclid", dAclid);

				//框架按钮数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo VALUES('F2','" + tgctp04.GetColValString("FORM_NO") + "'," +
					dAclid.ToString() + ",'查询','A','" + appName + "','" + (CString)s.userid + "',' ')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//框架按钮资源数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo_res VALUES('zh_Hans'," + dAclid.ToString() + ", '查询')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//事件配置表数据新增
				tgctp07.SetColVal("ID", dAclid);
				tgctp07.SetColVal("SEQ_NO", (CDecimal)seqNo++);
				tgctp07.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp07.SetColVal("NAME", "F2");
				tgctp07.SetColVal("DESCRIPTION", "查询");
				tgctp07.SetColVal("OPERATE_TYPE", "01");

				if (tgctp07.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//操作配置表数据新增
				tgctp05.SetColVal("ID", dAclid);
				tgctp05.SetColVal("SEQ_NO", (CDecimal)1);
				tgctp05.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp05.SetColVal("NAME", "F2");
				tgctp05.SetColVal("OPERATE_TYPE", "01");
				tgctp05.SetColVal("NOW_ROW", GetTrackSeqNo("GCTP_VTABLE_ROWID", 20, conn));
				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_FUNCTION");
				tgctp05.SetColVal("ITEM_CVALUE", "01");

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_PAGE_ID");
				tgctp05.SetColVal("ITEM_CVALUE", "1");

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_CFGITM_NAME");
				tgctp05.SetColVal("ITEM_CVALUE", tgctp04.GetColValString("CFGITM_NAME"));

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//输入参数配置表数据新增
				tgctp06.SetColVal("ID", dAclid);
				tgctp06.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp06.SetColVal("NOW_ROW", tgctp05.GetColValString("NOW_ROW"));
				tgctp06.SetColVal("PAGE_ID", "1");
				tgctp06.SetColVal("HANDLE_DIV", "0");
				tgctp06.SetColVal("PROC_SEQ_NO", (CDecimal)1);
				tgctp06.SetColVal("PARA_TYPE", "01");
				tgctp06.SetColVal("CFGITM_NAME", tgctp04.GetColValString("CFGITM_NAME"));
				tgctp06.SetColVal("DATA_ORIGIN", "01");
				tgctp06.SetColVal("ITEM_MUST_FLAG", "0");
				tgctp06.SetColVal("OPERATE_OBJECT", " ");
				tgctp06.SetColVal("OPERATE_MODE", " ");

				if (tgctp06.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//输出参数配置表数据新增
				tgctp06.SetColVal("HANDLE_DIV", "1");
				tgctp06.SetColVal("DATA_ORIGIN", " ");
				tgctp06.SetColVal("OBJECT_AREA", "02");
				tgctp06.SetColVal("SHOW_FLAG", "0");

				if (tgctp06.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}

			if (GetColValueC(bcls_rec->Tables[i], 0, "FUNC_DIV") == "1")
			{
				//F3 新增按钮
				dAclid = 1000000000 + GetSeqence("MMTP_BUTTON_SEQ_NO", conn);
				PrintLog("dAclid", dAclid);

				//框架按钮数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo VALUES('F3','" + tgctp04.GetColValString("FORM_NO") + "'," +
					dAclid.ToString() + ",'新增','B','" + appName + "','" + (CString)s.userid + "',' ')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//框架按钮资源数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo_res VALUES('zh_Hans'," + dAclid.ToString() + ", '新增')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//事件配置表数据新增
				tgctp07.SetColVal("ID", dAclid);
				tgctp07.SetColVal("SEQ_NO", (CDecimal)seqNo++);
				tgctp07.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp07.SetColVal("NAME", "F3");
				tgctp07.SetColVal("DESCRIPTION", "新增");
				tgctp07.SetColVal("OPERATE_TYPE", "02");

				if (tgctp07.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//操作配置表数据新增
				tgctp05.ClearDataRow();
				tgctp05.SetColVal("ID", dAclid);
				tgctp05.SetColVal("SEQ_NO", (CDecimal)1);
				tgctp05.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp05.SetColVal("NAME", "F3");
				tgctp05.SetColVal("OPERATE_TYPE", "02");
				tgctp05.SetColVal("NOW_ROW", GetTrackSeqNo("GCTP_VTABLE_ROWID", 20, conn));
				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_FUNCTION");
				tgctp05.SetColVal("ITEM_CVALUE", "02");

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_PAGE_ID");
				tgctp05.SetColVal("ITEM_CVALUE", "1");

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_CFGITM_NAME");
				tgctp05.SetColVal("ITEM_CVALUE", tgctp04.GetColValString("CFGITM_NAME"));

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//输入参数配置表数据新增
				tgctp06.SetColVal("ID", dAclid);
				tgctp06.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp06.SetColVal("NOW_ROW", tgctp05.GetColValString("NOW_ROW"));
				tgctp06.SetColVal("PAGE_ID", "1");
				tgctp06.SetColVal("HANDLE_DIV", "0");
				tgctp06.SetColVal("PROC_SEQ_NO", (CDecimal)1);
				tgctp06.SetColVal("PARA_TYPE", "01");
				tgctp06.SetColVal("CFGITM_NAME", tgctp04.GetColValString("CFGITM_NAME"));
				tgctp06.SetColVal("DATA_ORIGIN", "02");
				tgctp06.SetColVal("ITEM_MUST_FLAG", "0");
				tgctp06.SetColVal("OPERATE_OBJECT", "0");
				tgctp06.SetColVal("OPERATE_MODE", "1");

				if (tgctp06.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//输出参数配置表数据新增
				tgctp06.SetColVal("HANDLE_DIV", "1");
				tgctp06.SetColVal("DATA_ORIGIN", " ");
				tgctp06.SetColVal("OBJECT_AREA", "02");
				tgctp06.SetColVal("SHOW_FLAG", "0");

				if (tgctp06.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//F4 修改按钮
				dAclid = 1000000000 + GetSeqence("MMTP_BUTTON_SEQ_NO", conn);
				PrintLog("dAclid", dAclid);

				//框架按钮数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo VALUES('F4','" + tgctp04.GetColValString("FORM_NO") + "'," +
					dAclid.ToString() + ",'修改','B','" + appName + "','" + (CString)s.userid + "',' ')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//框架按钮资源数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo_res VALUES('zh_Hans'," + dAclid.ToString() + ", '修改')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//事件配置表数据新增
				tgctp07.SetColVal("ID", dAclid);
				tgctp07.SetColVal("SEQ_NO", (CDecimal)seqNo++);
				tgctp07.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp07.SetColVal("NAME", "F4");
				tgctp07.SetColVal("DESCRIPTION", "修改");
				tgctp07.SetColVal("OPERATE_TYPE", "02");

				if (tgctp07.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//操作配置表数据新增
				tgctp05.ClearDataRow();
				tgctp05.SetColVal("ID", dAclid);
				tgctp05.SetColVal("SEQ_NO", (CDecimal)1);
				tgctp05.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp05.SetColVal("NAME", "F4");
				tgctp05.SetColVal("OPERATE_TYPE", "02");
				tgctp05.SetColVal("NOW_ROW", GetTrackSeqNo("GCTP_VTABLE_ROWID", 20, conn));
				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_FUNCTION");
				tgctp05.SetColVal("ITEM_CVALUE", "03");

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_PAGE_ID");
				tgctp05.SetColVal("ITEM_CVALUE", "1");

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_CFGITM_NAME");
				tgctp05.SetColVal("ITEM_CVALUE", tgctp04.GetColValString("CFGITM_NAME"));

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//输入参数配置表数据新增
				tgctp06.SetColVal("ID", dAclid);
				tgctp06.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp06.SetColVal("NOW_ROW", tgctp05.GetColValString("NOW_ROW"));
				tgctp06.SetColVal("PAGE_ID", "1");
				tgctp06.SetColVal("HANDLE_DIV", "0");
				tgctp06.SetColVal("PROC_SEQ_NO", (CDecimal)1);
				tgctp06.SetColVal("PARA_TYPE", "01");
				tgctp06.SetColVal("CFGITM_NAME", tgctp04.GetColValString("CFGITM_NAME"));
				tgctp06.SetColVal("DATA_ORIGIN", "02");
				tgctp06.SetColVal("ITEM_MUST_FLAG", "0");
				tgctp06.SetColVal("OPERATE_OBJECT", "0");
				tgctp06.SetColVal("OPERATE_MODE", "0");

				if (tgctp06.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//输出参数配置表数据新增
				tgctp06.SetColVal("HANDLE_DIV", "1");
				tgctp06.SetColVal("DATA_ORIGIN", " ");
				tgctp06.SetColVal("OBJECT_AREA", "02");
				tgctp06.SetColVal("SHOW_FLAG", "0");

				if (tgctp06.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//F5 删除按钮
				dAclid = 1000000000 + GetSeqence("MMTP_BUTTON_SEQ_NO", conn);
				PrintLog("dAclid", dAclid);

				//框架按钮数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo VALUES('F5','" + tgctp04.GetColValString("FORM_NO") + "'," +
					dAclid.ToString() + ",'删除','B','" + appName + "','" + (CString)s.userid + "',' ')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//框架按钮资源数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo_res VALUES('zh_Hans'," + dAclid.ToString() + ", '删除')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//事件配置表数据新增
				tgctp07.SetColVal("ID", dAclid);
				tgctp07.SetColVal("SEQ_NO", (CDecimal)seqNo++);
				tgctp07.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp07.SetColVal("NAME", "F5");
				tgctp07.SetColVal("DESCRIPTION", "删除");
				tgctp07.SetColVal("OPERATE_TYPE", "02");

				if (tgctp07.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//操作配置表数据新增
				tgctp05.ClearDataRow();
				tgctp05.SetColVal("ID", dAclid);
				tgctp05.SetColVal("SEQ_NO", (CDecimal)1);
				tgctp05.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp05.SetColVal("NAME", "F5");
				tgctp05.SetColVal("OPERATE_TYPE", "02");
				tgctp05.SetColVal("NOW_ROW", GetTrackSeqNo("GCTP_VTABLE_ROWID", 20, conn));
				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_FUNCTION");
				tgctp05.SetColVal("ITEM_CVALUE", "04");

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_PAGE_ID");
				tgctp05.SetColVal("ITEM_CVALUE", "1");

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tgctp05.SetColVal("ITEM_ENAME", "OPERATE_CFGITM_NAME");
				tgctp05.SetColVal("ITEM_CVALUE", tgctp04.GetColValString("CFGITM_NAME"));

				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//输入参数配置表数据新增
				tgctp06.SetColVal("ID", dAclid);
				tgctp06.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
				tgctp06.SetColVal("NOW_ROW", tgctp05.GetColValString("NOW_ROW"));
				tgctp06.SetColVal("PAGE_ID", "1");
				tgctp06.SetColVal("HANDLE_DIV", "0");
				tgctp06.SetColVal("PROC_SEQ_NO", (CDecimal)1);
				tgctp06.SetColVal("PARA_TYPE", "01");
				tgctp06.SetColVal("CFGITM_NAME", tgctp04.GetColValString("CFGITM_NAME"));
				tgctp06.SetColVal("DATA_ORIGIN", "02");
				tgctp06.SetColVal("ITEM_MUST_FLAG", "0");
				tgctp06.SetColVal("OPERATE_OBJECT", "0");
				tgctp06.SetColVal("OPERATE_MODE", "0");

				if (tgctp06.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				
			}
			else if (GetColValueC(bcls_rec->Tables[i], 0, "FUNC_DIV") == "2")
			{
				//F3 保存按钮
				dAclid = 1000000000 + GetSeqence("MMTP_BUTTON_SEQ_NO", conn);
				PrintLog("dAclid", dAclid);

				//框架按钮数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo VALUES('F3','" + tgctp04.GetColValString("FORM_NO") + "'," +
					dAclid.ToString() + ",'保存','A','" + appName + "','" + (CString)s.userid + "',' ')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//框架按钮资源数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo_res VALUES('zh_Hans'," + dAclid.ToString() + ", '保存')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();
			}

			if (GetColValueC(bcls_rec->Tables[i], 0, "FUNC_DIV") == "0" ||
				GetColValueC(bcls_rec->Tables[i], 0, "FUNC_DIV") == "1" ||
				GetColValueC(bcls_rec->Tables[i], 0, "FUNC_DIV") == "2")
			{
				//F11 重载按钮
				PrintLog("****** 重载,配置按钮");
				dAclid = 1000000000 + GetSeqence("MMTP_BUTTON_SEQ_NO", conn);
				PrintLog("dAclid", dAclid);

				//框架按钮数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo VALUES('F11','" + tgctp04.GetColValString("FORM_NO") + "'," +
					dAclid.ToString() + ",'重载','A','" + appName + "','" + (CString)s.userid + "',' ')";
				PrintLog("sqlstr", sqlstr);
				cmd_ins.SetCommandText(sqlstr);
				cmd_ins.ExecuteNonQuery();
				cmd_ins.Close();

				//框架按钮资源数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo_res VALUES('zh_Hans'," + dAclid.ToString() + ", '重载')";
				PrintLog("sqlstr", sqlstr);
				cmd_ins.SetCommandText(sqlstr);
				cmd_ins.ExecuteNonQuery();
				cmd_ins.Close();

				//F12 配置按钮
				dAclid = 1000000000 + GetSeqence("MMTP_BUTTON_SEQ_NO", conn);
				PrintLog("dAclid", dAclid);

				//框架按钮数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo VALUES('F12','" + tgctp04.GetColValString("FORM_NO") + "'," +
					dAclid.ToString() + ",'配置','A','" + appName + "','" + (CString)s.userid + "',' ')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				//框架按钮资源数据表新增
				sqlstr = "INSERT INTO tesbuttonresinfo_res VALUES('zh_Hans'," + dAclid.ToString() + ", '配置')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();
			}

			//清空不存在id的框架按钮表数据
			sqlstr = "DELETE FROM tesbuttonresinfo_res WHERE aclid IN (SELECT aclid FROM tesbuttonresinfo WHERE fname = '" +
				tgctp04.GetColValString("FORM_NO") + "') AND aclid NOT IN (SELECT id FROM tgctp07 WHERE form_no = '" +
				tgctp04.GetColValString("FORM_NO") + "')";
			cmd.SetCommandText(sqlstr);
			PrintLog("sqlstr", sqlstr);
			cmd.ExecuteNonQuery();
			cmd.Close();

			sqlstr = "DELETE FROM tesbuttonresinfo WHERE fname = '" + tgctp04.GetColValString("FORM_NO") + "' AND"
				" aclid NOT IN (SELECT id FROM tgctp07 WHERE form_no = '" + tgctp04.GetColValString("FORM_NO") + "')";
			cmd.SetCommandText(sqlstr);
			PrintLog("sqlstr", sqlstr);
			cmd.ExecuteNonQuery();
			cmd.Close();
		}
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


