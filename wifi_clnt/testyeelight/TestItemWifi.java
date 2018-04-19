/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package testyeelight;

import java.io.Serializable;
import java.util.Date;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.xml.bind.annotation.XmlRootElement;

/**
 *
 * @author GUOWEI
 */
@Entity
@Table(name = "test_item_wifi")
@XmlRootElement
@NamedQueries({
    @NamedQuery(name = "TestItemWifi.findAll", query = "SELECT t FROM TestItemWifi t"),
    @NamedQuery(name = "TestItemWifi.findById", query = "SELECT t FROM TestItemWifi t WHERE t.id = :id"),
    @NamedQuery(name = "TestItemWifi.findByMoCode", query = "SELECT t FROM TestItemWifi t WHERE t.moCode = :moCode"),
    @NamedQuery(name = "TestItemWifi.findByItemModel", query = "SELECT t FROM TestItemWifi t WHERE t.itemModel = :itemModel"),
    @NamedQuery(name = "TestItemWifi.findByMacAddr", query = "SELECT t FROM TestItemWifi t WHERE t.macAddr = :macAddr"),
    @NamedQuery(name = "TestItemWifi.findByFirmwareVer", query = "SELECT t FROM TestItemWifi t WHERE t.firmwareVer = :firmwareVer"),
    @NamedQuery(name = "TestItemWifi.findByBrs", query = "SELECT t FROM TestItemWifi t WHERE t.brs = :brs"),
    @NamedQuery(name = "TestItemWifi.findByBrf", query = "SELECT t FROM TestItemWifi t WHERE t.brf = :brf"),
    @NamedQuery(name = "TestItemWifi.findByAvgRssi", query = "SELECT t FROM TestItemWifi t WHERE t.avgRssi = :avgRssi"),
    @NamedQuery(name = "TestItemWifi.findByMinRssi", query = "SELECT t FROM TestItemWifi t WHERE t.minRssi = :minRssi"),
    @NamedQuery(name = "TestItemWifi.findByMaxRssi", query = "SELECT t FROM TestItemWifi t WHERE t.maxRssi = :maxRssi"),
    @NamedQuery(name = "TestItemWifi.findByPacketSend", query = "SELECT t FROM TestItemWifi t WHERE t.packetSend = :packetSend"),
    @NamedQuery(name = "TestItemWifi.findByPacketRecv", query = "SELECT t FROM TestItemWifi t WHERE t.packetRecv = :packetRecv"),
    @NamedQuery(name = "TestItemWifi.findByTs", query = "SELECT t FROM TestItemWifi t WHERE t.ts = :ts")})
public class TestItemWifi implements Serializable {
    private static final long serialVersionUID = 1L;
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Basic(optional = false)
    @Column(name = "ID")
    private Integer id;
    @Column(name = "mo_code")
    private String moCode;
    @Column(name = "item_model")
    private String itemModel;
    @Column(name = "mac_addr")
    private String macAddr;
    @Column(name = "firmware_ver")
    private String firmwareVer;
    @Column(name = "brs")
    private String brs;
    @Column(name = "brf")
    private String brf;
    @Column(name = "avg_rssi")
    private String avgRssi;
    @Column(name = "min_rssi")
    private String minRssi;
    @Column(name = "max_rssi")
    private String maxRssi;
    @Column(name = "packet_send")
    private String packetSend;
    @Column(name = "packet_recv")
    private String packetRecv;
    @Column(name="isconfirm")
    private Character isConfirm;
    @Column(name = "ts",insertable=false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date ts;
    @JoinColumn(name = "H_ID", referencedColumnName = "ID")
    @ManyToOne
    private TestItemH hId;

    public TestItemWifi() {
    }

    public TestItemWifi(Integer id) {
        this.id = id;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getMoCode() {
        return moCode;
    }

    public void setMoCode(String moCode) {
        this.moCode = moCode;
    }

    public String getItemModel() {
        return itemModel;
    }

    public void setItemModel(String itemModel) {
        this.itemModel = itemModel;
    }

    public String getMacAddr() {
        return macAddr;
    }

    public void setMacAddr(String macAddr) {
        this.macAddr = macAddr;
    }

    public String getFirmwareVer() {
        return firmwareVer;
    }

    public void setFirmwareVer(String firmwareVer) {
        this.firmwareVer = firmwareVer;
    }

    public String getBrs() {
        return brs;
    }

    public void setBrs(String brs) {
        this.brs = brs;
    }

    public String getBrf() {
        return brf;
    }

    public void setBrf(String brf) {
        this.brf = brf;
    }

    public String getAvgRssi() {
        return avgRssi;
    }

    public void setAvgRssi(String avgRssi) {
        this.avgRssi = avgRssi;
    }

    public String getMinRssi() {
        return minRssi;
    }

    public void setMinRssi(String minRssi) {
        this.minRssi = minRssi;
    }

    public String getMaxRssi() {
        return maxRssi;
    }

    public void setMaxRssi(String maxRssi) {
        this.maxRssi = maxRssi;
    }

    public String getPacketSend() {
        return packetSend;
    }

    public void setPacketSend(String packetSend) {
        this.packetSend = packetSend;
    }

    public String getPacketRecv() {
        return packetRecv;
    }

    public void setPacketRecv(String packetRecv) {
        this.packetRecv = packetRecv;
    }

    public Date getTs() {
        return ts;
    }

    public void setTs(Date ts) {
        this.ts = ts;
    }

    public TestItemH getHId() {
        return hId;
    }

    public void setHId(TestItemH hId) {
        this.hId = hId;
    }

    public Character getIsConfirm() {
        return isConfirm;
    }

    public void setIsConfirm(Character isConfirm) {
        this.isConfirm = isConfirm;
    }
    
    @Override
    public int hashCode() {
        int hash = 0;
        hash += (id != null ? id.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof TestItemWifi)) {
            return false;
        }
        TestItemWifi other = (TestItemWifi) object;
        if ((this.id == null && other.id != null) || (this.id != null && !this.id.equals(other.id))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "testyeelight.TestItemWifi[ id=" + id + " ]";
    }
    
}
