import { styled } from 'styled-components';

export const FormContainer = styled.form`
  display: flex;
  flex-direction: column;
  justify-content: center;
  width: 100%;
  height: 100%;
  position: relative;
`;

export const Title = styled.h2`
  // margin-bottom: .1rem;
  margin-bottom: -0.2rem;
  text-align: center;
  color: #121212;
  font-size: 1.7rem;
  font-weight: 550;
  text-transform: uppercase;
`;

export const Subtitle = styled.h3`
  margin-bottom: 1.5rem;
  text-align: center;
  color: #555;
  font-size: .9rem;
`;

export const Input = styled.input`
  padding-top: .5rem;
  padding-bottom: .5rem;
  padding-left: 1rem;
  padding-right: .3rem;
  border: 1px solid #ccc;
  border-radius: 8px;
  font-size: 1rem;
  &:focus {
    outline: none;
    border-color: #ff8000ff;
  }
`;

export const Label = styled.label`
  margin-bottom: 0.5rem;
  font-weight: 550;
  color: #333;
  
  &:not(:first-child) {
    margin-top: 1rem;
  }
`;

export const Button = styled.button<{ $backgroundColor?: string, $color?: string }>`
  background-color: ${(p) => p.$backgroundColor || '#ff8000ff'};
  color: ${(p) => p.color || '#fff'};
  border: none;
  border-radius: 8px;
  border: 1px solid #ccc;
  padding: 0.75rem;
  font-size: 1rem;
  cursor: pointer;
  transition: background-color 0.2s;
  &:hover {
    opacity: 0.9;
  }
`;

export const Checkbox = styled.input`
  margin-top: 1rem;
  margin-bottom: 1rem;
  margin-right: .3rem;

`;

export const ErrorText = styled.p`
  color: #d32f2f;
  font-size: 0.9rem;
  text-align: center;
`;

export const LeftContainer = styled.div`
  display: flex;
  justify-content: center;
  // flex-direction: column;
  // align-items: center;
  // justify-content: center;
  width: 600px;
  height: 90vh;
  background-color: #fff;
  border-top-left-radius: 8px;
  border-bottom-left-radius: 8px;
  padding: 4rem;
  padding-top: 3rem;
  overflow-y: auto;

  @media (max-height: 450px) { 
    display: none;
  }

  @media (max-width: 1000px) { 
    padding-left: 1.7rem;
    padding-right: 1.7rem;
  }

  @media (max-width: 800px) {
    width: 100%;
    padding: 3rem; 

    // h2 { 
    //   font-size: 1.5rem;
    // }

    // h3 { 
    //   font-size: 0.9rem;
    //   margin-bottom: .3rem;
    // }

    input[type="checkbox"] { 
      transform: scale(0.8);
      margin-top: 0.5rem;
      margin-bottom: 0.5rem;
      margin-right: 0.3rem;
    }

    input[type="text"], input[type="password"] {
      width: 100%;
      font-size: 0.9rem;  
    }

    button { 
      width: 100%;
      font-size: 0.9rem;
    }
  }
`;

export const RightContainer = styled.div`
  width: 800px;
  height: 90vh;
  background-color: #fb7805ff;
  border-top-right-radius: 8px;
  border-bottom-right-radius: 8px;

  @media (max-width: 800px) {
    display: none;
  }

  @media (max-height: 450px) { 
    display: none;
  }
`;

export const CheckBoxWrapper = styled.div`
  font-size: .9rem;
`;

export const ProgressWrapper = styled.div`
  width: 100%;
  height: 4px;
  background-color: #ccc;
  border-radius: 2px;
  margin-top: 1rem;
  margin-bottom: 1rem;
`;

export const ProgressBar = styled.div<{ progress: number }>`
  width: ${(p) => p.progress}%;
  height: 100%;
  background-color: #fc5d08;
  border-radius: 2px;
  transition: width 0.1s ease-in-out;
`;