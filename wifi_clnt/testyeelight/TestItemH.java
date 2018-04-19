/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package testyeelight;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.XmlTransient;

/**
 *
 * @author GUOWEI
 */
@Entity
@Table(name = "test_item_h")
@XmlRootElement
@NamedQueries({
    @NamedQuery(name = "TestItemH.findAll", query = "SELECT t FROM TestItemH t"),
    @NamedQuery(name = "TestItemH.findById", query = "SELECT t FROM TestItemH t WHERE t.id = :id"),
    @NamedQuery(name = "TestItemH.findByShop", query = "SELECT t FROM TestItemH t WHERE t.shop = :shop"),
    @NamedQuery(name = "TestItemH.findByProductLine", query = "SELECT t FROM TestItemH t WHERE t.productLine = :productLine"),
    @NamedQuery(name = "TestItemH.findByTestItem", query = "SELECT t FROM TestItemH t WHERE t.testItem = :testItem"),
    @NamedQuery(name = "TestItemH.findByEquipName", query = "SELECT t FROM TestItemH t WHERE t.equipName = :equipName"),
    @NamedQuery(name = "TestItemH.findByTestPersonal", query = "SELECT t FROM TestItemH t WHERE t.testPersonal = :testPersonal"),
    @NamedQuery(name = "TestItemH.findByStartTime", query = "SELECT t FROM TestItemH t WHERE t.startTime = :startTime"),
    @NamedQuery(name = "TestItemH.findByEndTime", query = "SELECT t FROM TestItemH t WHERE t.endTime = :endTime"),
    @NamedQuery(name = "TestItemH.findByTs", query = "SELECT t FROM TestItemH t WHERE t.ts = :ts")})
public class TestItemH implements Serializable {
    private static final long serialVersionUID = 1L;
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Basic(optional = false)
    @Column(name = "ID")
    private Integer id;
    @Column(name = "shop")
    private String shop;
    @Column(name = "productLine")
    private String productLine;
    @Column(name = "test_item")
    private String testItem;
    @Column(name = "equip_name")
    private String equipName;
    @Column(name = "test_personal")
    private String testPersonal;
    @Column(name = "start_time")
    private String startTime;
    @Column(name = "end_time")
    private String endTime;
    @Column(name = "ts",insertable=false)
    @Temporal(TemporalType.TIMESTAMP)
    private Date ts;
    @OneToMany(mappedBy = "hId" ,cascade = {CascadeType.MERGE,CascadeType.PERSIST})
    private List<TestItemWifi> testItemWifiList = new ArrayList<>();

    public TestItemH() {
    }

    public TestItemH(Integer id) {
        this.id = id;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getShop() {
        return shop;
    }

    public void setShop(String shop) {
        this.shop = shop;
    }

    public String getProductLine() {
        return productLine;
    }

    public void setProductLine(String productLine) {
        this.productLine = productLine;
    }

    public String getTestItem() {
        return testItem;
    }

    public void setTestItem(String testItem) {
        this.testItem = testItem;
    }

    public String getEquipName() {
        return equipName;
    }

    public void setEquipName(String equipName) {
        this.equipName = equipName;
    }

    public String getTestPersonal() {
        return testPersonal;
    }

    public void setTestPersonal(String testPersonal) {
        this.testPersonal = testPersonal;
    }

    public String getStartTime() {
        return startTime;
    }

    public void setStartTime(String startTime) {
        this.startTime = startTime;
    }

    public String getEndTime() {
        return endTime;
    }

    public void setEndTime(String endTime) {
        this.endTime = endTime;
    }

    public Date getTs() {
        return ts;
    }

    public void setTs(Date ts) {
        this.ts = ts;
    }

    @XmlTransient
    public List<TestItemWifi> getTestItemWifiList() {
        return testItemWifiList;
    }

    public void setTestItemWifiList(List<TestItemWifi> testItemWifiList) {
        this.testItemWifiList = testItemWifiList;
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
        if (!(object instanceof TestItemH)) {
            return false;
        }
        TestItemH other = (TestItemH) object;
        if ((this.id == null && other.id != null) || (this.id != null && !this.id.equals(other.id))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "testyeelight.TestItemH[ id=" + id + " ]";
    }
    
}
