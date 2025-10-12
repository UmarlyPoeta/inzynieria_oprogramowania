import { ReactLogo } from '@/assets';
import './Siderbar.css';

const Sidebar = () => {
  return (
    <aside className="sidebar">
      <div className="sidebar__logo-container">
        <img src={ReactLogo} alt="logo" className="sidebar__logo" />
        NetSimCPP
      </div>

      <ul className="sidebar__menu">
        <li className="sidebar__menu-item-header">Section 1</li>
        <li className="sidebar__menu-item">
          <a href="#" className="sidebar__menu-link">
          {/* <Link to="/s1/s1" className="sidebar__menu-link"> */}
            <span>Element One</span>
          {/* </Link> */}
          </a>
        </li>
        <li className="sidebar__menu-item">
          <a href="#" className="sidebar__menu-link">
          {/* <Link to="/s1/s1" className="sidebar__menu-link"> */}
            <span>Element Two</span>
          {/* </Link> */}
          </a>
        </li>
        <li className="sidebar__menu-item">
          <a href="#" className="sidebar__menu-link">
          {/* <Link to="/s1/s1" className="sidebar__menu-link"> */}
            <span>Element Three</span>
          {/* </Link> */}
          </a>
        </li>
        <li className="sidebar__menu-item-header">Section 2</li>
        <li className="sidebar__menu-item">
            <a href="#" className="sidebar__menu-link">
          {/* <Link to="/s2/s1" className="sidebar__menu-link"> */}
            {/* ikona */}
            <span>Element One</span>
          {/* </Link> */}
          </a>
        </li>
        <li className="sidebar__menu-item">
          <a href="#" className="sidebar__menu-link">
          {/* <Link to="/s1/s1" className="sidebar__menu-link"> */}
            <span>Element Two</span>
          {/* </Link> */}
          </a>
        </li>
        <li className="sidebar__menu-item">
          <a href="#" className="sidebar__menu-link">
          {/* <Link to="/s1/s1" className="sidebar__menu-link"> */}
            <span>Element Three</span>
          {/* </Link> */}
          </a>
        </li>
        <li className="sidebar__menu-item">
          <a href="#" className="sidebar__menu-link">
          {/* <Link to="/s1/s1" className="sidebar__menu-link"> */}
            <span>Element Four</span>
          {/* </Link> */}
          </a>
        </li>
      </ul>
    </aside>
  );
};

export default Sidebar;







