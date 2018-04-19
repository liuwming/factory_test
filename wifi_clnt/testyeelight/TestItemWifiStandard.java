/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package testyeelight;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.xml.bind.annotation.XmlRootElement;

/**
 *
 * @author GUOWEI
 */
@Entity
@Table(name = "test_item_wifi_standard")
@XmlRootElement
@NamedQueries({
    @NamedQuery(name = "TestItemWifiStandard.findAll", query = "SELECT t FROM TestItemWifiStandard t"),
    @NamedQuery(name = "TestItemWifiStandard.findById", query = "SELECT t FROM TestItemWifiStandard t WHERE t.id = :id"),
    @NamedQuery(name = "TestItemWifiStandard.findByModel", query = "SELECT t FROM TestItemWifiStandard t WHERE t.model = :model"),
    @NamedQuery(name = "TestItemWifiStandard.findByRssiStandard", query = "SELECT t FROM TestItemWifiStandard t WHERE t.rssiStandard = :rssiStandard"),
    @NamedQuery(name = "TestItemWifiStandard.findByMinRssiStandard", query = "SELECT t FROM TestItemWifiStandard t WHERE t.minRssiStandard = :minRssiStandard"),
    @NamedQuery(name = "TestItemWifiStandard.findByMaxRssiStandard", query = "SELECT t FROM TestItemWifiStandard t WHERE t.maxRssiStandard = :maxRssiStandard"),
    @NamedQuery(name = "TestItemWifiStandard.findByPacketSendStandard", query = "SELECT t FROM TestItemWifiStandard t WHERE t.packetSendStandard = :packetSendStandard"),
    @NamedQuery(name = "TestItemWifiStandard.findByPacketRecvStandard", query = "SELECT t FROM TestItemWifiStandard t WHERE t.packetRecvStandard = :packetRecvStandard")})
public class TestItemWifiStandard implements Serializable {
    private static final long serialVersionUID = 1L;
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Basic(optional = false)
    @Column(name = "ID")
    private Integer id;
    @Column(name = "model")
    private String model;
    @Column(name = "rssi_standard")
    private String rssiStandard;
    @Column(name = "min_rssi_standard")
    private String minRssiStandard;
    @Column(name = "max_rssi_standard")
    private String maxRssiStandard;
    @Column(name = "packet_send_standard")
    private String packetSendStandard;
    @Column(name = "packet_recv_standard")
    private String packetRecvStandard;

    public TestItemWifiStandard() {
    }

    public TestItemWifiStandard(Integer id) {
        this.id = id;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public String getRssiStandard() {
        return rssiStandard;
    }

    public void setRssiStandard(String rssiStandard) {
        this.rssiStandard = rssiStandard;
    }

    public String getMinRssiStandard() {
        return minRssiStandard;
    }

    public void setMinRssiStandard(String minRssiStandard) {
        this.minRssiStandard = minRssiStandard;
    }

    public String getMaxRssiStandard() {
        return maxRssiStandard;
    }

    public void setMaxRssiStandard(String maxRssiStandard) {
        this.maxRssiStandard = maxRssiStandard;
    }

    public String getPacketSendStandard() {
        return packetSendStandard;
    }

    public void setPacketSendStandard(String packetSendStandard) {
        this.packetSendStandard = packetSendStandard;
    }

    public String getPacketRecvStandard() {
        return packetRecvStandard;
    }

    public void setPacketRecvStandard(String packetRecvStandard) {
        this.packetRecvStandard = packetRecvStandard;
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
        if (!(object instanceof TestItemWifiStandard)) {
            return false;
        }
        TestItemWifiStandard other = (TestItemWifiStandard) object;
        if ((this.id == null && other.id != null) || (this.id != null && !this.id.equals(other.id))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "testyeelight.TestItemWifiStandard[ id=" + id + " ]";
    }
    
}
