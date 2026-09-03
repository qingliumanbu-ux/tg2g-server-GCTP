/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      admin
Version:     1.0
Date:        2018-12-19 08:35:01
Description: 数据新增函数
**************************************************/

#include "CDynaTable2.h"

BM2_FUNCTION_IMPORT
int f_gctp_getConfigData(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_gctp_dataInsert(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDecimal buttonNo = 0;
	CString sTableType = "";
	CString sTableName = "";
	CString sDataSource = "";
	CString sDataSet = "";
	CString sEventId = "";
	CString sEventDesc = "";
	CString sLogTableName = "";
	CString sFuncName = "";
	CString pkName1 = "";
	CString pkValue1 = "";
	CString pkName2 = "";
	CString pkValue2 = "";
	CString pkName3 = "";
	CString pkValue3 = "";

	int iInsFlag = 0;

	CDataTable dtMain;
	CDataTable dtMainItem;
	CDataTable dtCallButton;

	try
	{
		if (bcls_rec->Tables.Contains("DS_MAIN") && bcls_rec->Tables.Contains("DS_ITEM") &&
			bcls_rec->Tables.Contains("OPERATE_DO"))
		{
			dtMain.Copy(bcls_rec->Tables["DS_MAIN"]);
			dtMainItem.Copy(bcls_rec->Tables["DS_ITEM"]);
			dtCallButton.Copy(bcls_rec->Tables["OPERATE_DO"]);
		}
		else if (GetColValueC(bcls_rec->Tables[0], 0, "CFGITM_NAME").Trim() != "")
		{
			doFlag = f_gctp_getConfigData(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			dtMain.Copy(bcls_ret->Tables["DS_MAIN"]);
			dtMainItem.Copy(bcls_ret->Tables["DS_ITEM"]);
			dtCallButton.Copy(bcls_ret->Tables["OPERATE_DO"]);
		}
		else
		{
			strcpy(s.msg, "传入数据不完整");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sTableType = dtMain.Rows[0]["TABLE_TYPE"].ToString();
		if (sTableType.Trim() == "")
		{
			strcpy(s.msg, "没有配置数据表类型");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		PrintLog("sTableType", sTableType);

		sTableName = dtMain.Rows[0]["TABLE_NAME"].ToString();
		if (sTableName.Trim() == "")
		{
			strcpy(s.msg, "没有配置数据表名");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		PrintLog("sTableName", sTableName);

		if (dtCallButton.Rows.get_Count() > 0)
		{
			sFuncName = GetColValueC(dtCallButton, 0, "EDCALL_FUNC_NAME").Trim();
			pkName1 = GetColValueC(dtCallButton, 0, "EDCALL_PK1_NAME").Trim();
			pkValue1 = GetColValueC(dtCallButton, 0, "EDCALL_PK1_VALUE").Trim();
			pkName2 = GetColValueC(dtCallButton, 0, "EDCALL_PK2_NAME").Trim();
			pkValue2 = GetColValueC(dtCallButton, 0, "EDCALL_PK2_VALUE").Trim();
			pkName3 = GetColValueC(dtCallButton, 0, "EDCALL_PK3_NAME").Trim();
			pkValue3 = GetColValueC(dtCallButton, 0, "EDCALL_PK3_VALUE").Trim();

			sEventId = GetColValueC(dtCallButton, 0, "EVENT_ID").Trim();
			sEventDesc = GetColValueC(dtCallButton, 0, "EVENT_DESC").Trim();
			sLogTableName = GetColValueC(dtCallButton, 0, "LOG_TABLE_NAME").Trim();
		}

		buttonNo = dtCallButton.Rows[0]["OPERATE_BUTTON"].ToDecimal();
		PrintLog("buttonNo", buttonNo);

		sDataSource = "DEFAULT_ENABLE_STATE_" + buttonNo.ToString();
		sDataSet = "ENABLE_CTRL_STR_" + buttonNo.ToString();

		//定义数据表
		CDynaTable2 dynaTable(sTableName, conn);

		if (sTableType == "2")
		{

		}
		else
		{
			//获取前台传入新增数据
			if (bcls_rec->Tables.Contains("DATA_INS"))
			{
				dynaTable.CopyFrom(bcls_rec->Tables["DATA_INS"]);
			}
			else
			{
				dynaTable.CopyFrom(bcls_rec->Tables[0]);
			}

			for (int i = 0; i < dtMainItem.Rows.get_Count(); i++)
			{
				CString colName = dtMainItem.Rows[i]["COLUMN_NAME"].ToString();
				if (dtMainItem.Rows[i][sDataSource].ToString() == "3")
				{
					if (dtMainItem.Rows[i]["DATA_TYPE"].ToString() == "N")
					{
						if (dtMainItem.Rows[i][sDataSet].ToString().Trim() == "")
						{
							dynaTable.SetColValAllRow(colName, (CDecimal)0);
						}
						else
						{
							dynaTable.SetColValAllRow(colName, dtMainItem.Rows[i][sDataSet].ToDecimal());
						}
					}
					else
					{
						if (dtMainItem.Rows[i][sDataSet].ToString().Trim() == "")
						{
							dynaTable.SetColValAllRow(colName, " ");
						}
						else
						{
							dynaTable.SetColValAllRow(colName, dtMainItem.Rows[i][sDataSet].ToString());
						}
					}
				}
				else if (dtMainItem.Rows[i][sDataSource].ToString() == "4"  && dtMainItem.Rows[i][sDataSet].ToString().Trim() != "")
				{
					for (int j = 0; j < dynaTable.GetRowCount(); j++)
					{
						dynaTable.CopyColVal(colName, dtMainItem.Rows[i][sDataSet].ToString().Trim(), j);
					}
				}
				else if (dtMainItem.Rows[i][sDataSource].ToString() == "5")
				{
					PrintLog(sDataSet, dtMainItem.Rows[i][sDataSet].ToString());
					if (dtMainItem.Rows[i][sDataSet].ToString().Trim() == "")
					{
						strcpy(s.msg, "序列名未配置");
						throw CApplicationException(-1, s.msg, log.Location);
					}

					for (int j = 0; j < dynaTable.GetRowCount(); j++)
					{
						dynaTable.SetColVal(colName, GetSeqence(dtMainItem.Rows[i][sDataSet].ToString(), 0, conn), j);
					}
				}
				else if (dtMainItem.Rows[i][sDataSource].ToString() == "6")
				{
					PrintLog(sDataSet, dtMainItem.Rows[i][sDataSet].ToString());
					if (dtMainItem.Rows[i][sDataSet].ToString().Trim() == "")
					{
						strcpy(s.msg, "序列名未配置");
						throw CApplicationException(-1, s.msg, log.Location);
					}

					for (int j = 0; j < dynaTable.GetRowCount(); j++)
					{
						dynaTable.SetColVal(colName, GetTrackSeqNo(dtMainItem.Rows[i][sDataSet].ToString(), 20, conn), j);
					}
				}
				else if (dtMainItem.Rows[i][sDataSource].ToString() == "7")
				{
					dynaTable.SetColValAllRow(colName, CDateTime::Now().ToString("yyyyMMddHHmmss"));
				}
				else if (dtMainItem.Rows[i][sDataSource].ToString() == "8")
				{
					dynaTable.SetColValAllRow(colName, (CString)s.userid);
				}
				else if (dtMainItem.Rows[i][sDataSource].ToString() == "B")
				{
					dynaTable.SetColValAllRow(colName, (CString)s.username);
				}
				else if (dtMainItem.Rows[i][sDataSource].ToString() == "C")
				{
					if (dtMainItem.Rows[i]["DATA_TYPE"].ToString() == "D")
					{
						for (int j = 0; j < dynaTable.GetRowCount(); j++)
						{
							dynaTable.SetColVal(colName, (CDecimal)j + 1, j);
						}
					}
					else
					{
						for (int j = 0; j < dynaTable.GetRowCount(); j++)
						{
							dynaTable.SetColVal(colName, ((CDecimal)j + 1).ToString(), j);
						}
					}
				}
			}
		}

		//新增数据
		//dynaTable.Print();

		//设置EDCALL参数
		if (sFuncName != "")
		{
			strcpy(e.func_name[0], sFuncName);
			strcpy(e.pk_name[0], pkName1);
			strcpy(e.pk_val[0], pkValue1);
			strcpy(e.pk_name[1], pkName2);
			strcpy(e.pk_val[1], pkValue2);
			strcpy(e.pk_name[2], pkName3);
			strcpy(e.pk_val[2], pkValue3);

			//将ED结构压入 EIClass func_rec 中
			bcls_rec->SetED(e);

			bcls_rec->Tables.Add("EDCALL");
			dynaTable.CopyTo(bcls_rec->Tables["EDCALL"]);
			doFlag = f_epedcall(bcls_rec, bcls_ret);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			for (int i = 0; i < dtMainItem.Rows.get_Count(); i++)
			{
				CString colName = dtMainItem.Rows[i]["COLUMN_NAME"].ToString();
				if (dtMainItem.Rows[i][sDataSource].ToString() == "9")
				{
					for (int j = 0; j < bcls_rec->Tables["EDCALL"].Rows.get_Count(); j++)
					{
						dynaTable.CopyColVal(colName, bcls_rec->Tables["EDCALL"].Rows[j], j);
					}
				}
			}
		}

		if (dynaTable.Insert(iInsFlag) < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		if (sEventId != "" && sLogTableName != "")
		{
			PrintLog("sEventId", sEventId);
			PrintLog("sEventDesc", sEventDesc);
			PrintLog("sLogTableName", sLogTableName);

			CDynaTable2 logData(sLogTableName, conn);
			logData.CopyFrom(dynaTable.GetDataTable());
			for (int i = 0; i < logData.GetRowCount(); i++)
			{
				logData.SetColVal("EVENT_ID", sEventId, i);
				logData.SetColVal("EVENT_NAME", sEventDesc, i);
				logData.SetColVal("EVENT_DESC", sEventDesc, i);
				logData.SetColVal("FUNC_ID", (CString)s.svc_name, i);
				logData.SetColVal("FORM_NAME", (CString)s.formname, i);
				logData.SetColVal("RESUME_SEQ_NO", GetTrackSeqNo("MM00_RESUME_SEQ_NO", 20, conn), i);
			}

			if (logData.Insert() < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		dynaTable.CopyTo(bcls_ret->Tables[0]);

		if (GetColValueC(bcls_rec->Tables[0], 0, "DEBUG_FLAG") == "1")
		{
			strcpy(s.msg, "调试成功");
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
